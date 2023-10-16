#include "kernel/types.h"
#include "user.h"
#include "mine.h"

#define BUF_SIZE 100

int main(int argc, char *argv[])
{
    // copy argv
    char buf[BUF_SIZE];
    char *margv[ARGV_SIZE];
    for (int i = 0; i < argc; i++)
    {
        margv[i] = malloc(strlen(argv[i]) + 1);
        strcpy(margv[i], argv[i]);
    }

    // read args from stdin
    int read_num, read_total = 0;
    while ((read_num = read(STDIN_FILENO, buf + read_total, BUF_SIZE - read_total)) > 0)
        read_total += read_num;
    if (read_total >= BUF_SIZE)
        err("xargs: input too big\n");
    buf[read_total] = '\0';

    // parse the args
    for (int i = 0; i < read_total; i++)
    {
        if (buf[i] == ' ' || buf[i] == '\n') buf[i] = '\0';
        else if (i == 0 || buf[i - 1] == '\0')
            margv[argc++] = buf + i;
    }

    // echo hello too | xargs echo bye
    margv[argc] = NULL;

    // execute the program
    exec(margv[1], margv + 1);

    // Normally we don't get here
    err("xargs: unable to execute the program %s\n", margv[1]);

    // handle the compile error
    exit(-1);
}