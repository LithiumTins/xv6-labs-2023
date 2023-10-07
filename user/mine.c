#include "kernel/types.h"
#include "user.h"
#include "mine.h"

void err(char *fmt, ...)
{
    va_list va;

    va_start(va, fmt);
    vprintf(STDERR_FILENO, fmt, va);

    va_end(va);
    exit(-1);
}