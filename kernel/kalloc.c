// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem[NCPU];

// keep track of which cpu is using kalloc
int max_in_use = 0;

void
kinit()
{
  // initialize all locks
  for (int i = 0; i < NCPU; i++)
    initlock(&kmem[i].lock, "kmem");
  // allocate all physical memory to cpu 0
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;
  int id;

  push_off();
  id = cpuid();
  pop_off();

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem[id].lock);
  r->next = kmem[id].freelist;
  kmem[id].freelist = r;
  release(&kmem[id].lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;
  int id;

  // get cpu id and mark cpu as in use
  push_off();
  id = cpuid();
  pop_off();
  max_in_use = (max_in_use < id) ? id : max_in_use;

  acquire(&kmem[id].lock);
  r = kmem[id].freelist;
  if (r)
    kmem[id].freelist = r->next;
  release(&kmem[id].lock);

  if (r)
  {
    memset((char*)r, 5, PGSIZE); // fill with junk
    return (void*)r;
  }

  for (int i = 0; i <= max_in_use; i++)
  {
    if (i == id)
      continue;
    acquire(&kmem[i].lock);
    r = kmem[i].freelist;
    if (r)
      kmem[i].freelist = r->next;
    release(&kmem[i].lock);

    if (r)
    {
      memset((char*)r, 5, PGSIZE); // fill with junk
      return (void*)r;
    }
  }
  return 0;
}
