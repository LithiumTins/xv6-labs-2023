#include "kernel/types.h"
#include "user.h"
#include "mine.h"

#define BUF_SIZE 128

int main(int argc, char *argv[])
{
    int ppfd[2], cpfd[2], prfd, pwfd, crfd, cwfd, num;
    char buffer[BUF_SIZE];

    // create pipe
    if (pipe(ppfd) == -1 || pipe(cpfd) == -1)
        err("pipe fail\n");
    prfd = ppfd[0];
    pwfd = cpfd[1];
    crfd = cpfd[0];
    cwfd = ppfd[1];

    // create child process
    switch (fork())
    {
    case -1:
        err("fork fail\n");
    case 0:     // child
        if (close(prfd) == -1 || close(pwfd) == -1)
            err("child close fail\n");
        if ((num = read(crfd, buffer, BUF_SIZE)) == -1)
            err("child read fail\n");
        buffer[num] = '\0';
        printf("%d: received %s\n", getpid(), buffer);
        if (write(cwfd, "pong", 4) == -1)
            err("child write fail\n");
        break;
    default:    // parent
        if (close(crfd) == -1 || close(cwfd) == -1)
            err("parent close fail\n");
        if (write(pwfd, "ping", 4) == -1)
            err("parent write fail\n");
        if ((num = read(prfd, buffer, BUF_SIZE)) == -1)
            err("parent read fail\n");
        buffer[num] = '\0';
        printf("%d: received %s\n", getpid(), buffer);
        break;
    }

    exit(0);
}