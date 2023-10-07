#include <stdarg.h>

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

#define NULL ((void *)0)

#define ARGV_SIZE 20

// vprintf in user/printf.c
void vprintf(int fd, const char *fmt, va_list ap);

// defined in user/mine.c, printf info to stderr and exit
void err(char *fmt, ...);

// embed some system calls, defined in user/mine.c
