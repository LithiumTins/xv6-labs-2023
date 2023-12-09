#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "fcntl.h"

struct file {
  enum { FD_NONE, FD_PIPE, FD_INODE, FD_DEVICE } type;
  int ref; // reference count
  char readable;
  char writable;
  struct pipe *pipe; // FD_PIPE
  struct inode *ip;  // FD_INODE and FD_DEVICE
  uint off;          // FD_INODE
  short major;       // FD_DEVICE
};

struct sleeplock {
  uint locked;       // Is the lock held?
  struct spinlock lk; // spinlock protecting this sleep lock
  
  // For debugging:
  char *name;        // Name of lock.
  int pid;           // Process holding lock
};

struct inode {
  uint dev;           // Device number
  uint inum;          // Inode number
  int ref;            // Reference count
  struct sleeplock lock; // protects everything below here
  int valid;          // inode has been read from disk?

  short type;         // copy of disk inode
  short major;
  short minor;
  short nlink;
  uint size;
  uint addrs[13];
};

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_mmap(void)
{
  struct proc *p = myproc();

  uint64 addr;
  uint64 len;
  int prot;
  int flag;
  int fd;
  uint64 offset;

  argaddr(0, &addr);
  argaddr(1, &len);
  argint(2, &prot);
  argint(3, &flag);
  argint(4, &fd);
  argaddr(5, &offset);

  if (addr != 0)
    return -1;

  if ((prot & PROT_READ) == 0 && (prot & PROT_WRITE) == 0)
    return -1;

  if ((prot & PROT_WRITE) && !p->ofile[fd]->writable && flag != MAP_PRIVATE)
    return -1;

  if (flag != MAP_SHARED && flag != MAP_PRIVATE)
    return -1;

  if (offset != 0)
    return -1;

  for (int i = 0; i < 16; i++)
  {
    if (p->maps[i].valid == 0)
    {
      p->maps[i].valid = 1;
      p->maps[i].prot = prot;
      p->maps[i].flag = flag;
      p->maps[i].va = p->va;
      p->maps[i].f = p->ofile[fd];
      p->maps[i].f->ref++;
      ilock(p->maps[i].f->ip);
      p->maps[i].len = (len > p->maps[i].f->ip->size) ? p->maps[i].f->ip->size : len;
      iunlock(p->maps[i].f->ip);
      p->va += PGROUNDUP(len);
      return p->maps[i].va;
    }
  }

  return -1;
}

uint64
sys_munmap(void)
{
  struct proc *p = myproc();

  uint64 addr;
  uint64 len;

  argaddr(0, &addr);
  argaddr(1, &len);

  if (addr != PGROUNDDOWN(addr))
    return -1;

  for (int i = 0; i < 16; i++)
  {
    struct vma *m = &p->maps[i];
    if (!m->valid || m->va > addr || m->va + m->len <= addr) 
    {
      m = 0;
      continue;
    }
    if (m->flag == MAP_SHARED)
    {
      begin_op();
      ilock(m->f->ip);
      writei(m->f->ip, 1, addr, m->offset + addr - m->va, (len > m->len) ? m->len : len);
      iunlock(m->f->ip);
      end_op();
    }
    uvmdealloc(p->pagetable, addr + len, addr);
    if (addr == m->va)
    {
      m->len -= PGROUNDUP(len);
      m->va += PGROUNDUP(len);
    }
    else
    {
      m->len = addr - m->va;
    }

    if (m->len <= 0)
    {
      m->f->ref--;
      m->valid = 0;
    }
  }

  return 0;
}