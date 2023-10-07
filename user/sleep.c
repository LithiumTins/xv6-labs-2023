#include "kernel/types.h"
#include "user.h"
#include "mine.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
        err("usage: %s time\n", argv[0]);

    int ticks = atoi(argv[1]);
    sleep(ticks);
    printf("(nothing happens for a little while)\n");
    exit(0);
}