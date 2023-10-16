#include "kernel/types.h"
#include "user.h"
#include "mine.h"

int main(int argc, char *argv[])
{
    if (argc != 1)
        err("usage: %s\n", argv[0]);

    printf("uptime: %d\n", uptime());

    return 0;
}