#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "riscv.h"
#include "proc.h"
#include "defs.h"
#include "sysinfo.h"

uint64
sys_trace(void)
{
  int traced;

  argint(0, &traced);
  return trace(traced);
}

uint64
sys_sysinfo(void)
{
  struct sysinfo info;
  uint64 addr;
  struct proc *p = myproc();

  argaddr(0, &addr);
  sysinfo(&info);
  return copyout(p->pagetable, addr, (char *)&info, sizeof(info));
}

int sysinfo(struct sysinfo *info)
{
    info->freemem = kfreemem();
    info->nproc = usednum();

    return 0;
}

int trace(int traced)
{
  struct proc *p = myproc();

  p->traced |= traced;

  return 0;
}