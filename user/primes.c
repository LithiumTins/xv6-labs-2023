#include "kernel/types.h"
#include "user.h"
#include "mine.h"

int prime = 2, next = 0, pfd[2], rfd, wfd;

void test_prime(int num);

int main(int argc, char *argv[])
{
    printf("prime %d\n", prime);

    for (int i = 3; i <= 35; i++)
    {
        test_prime(i);
        if (prime != 2)     // not the first process
            break;
    }

    if (prime != 2)         // latter processes
    {
        int num;
        while(read(rfd, &num, sizeof(num)) > 0)
            test_prime(num);
        // previous process want to exit, ask next to exit first
        if (next)
        {
            if (close(wfd) == -1)
                err("primes: close() fail\n");
            int status;
            if (wait(&status) == -1)
                err("primes: wait() fail\n");
        }
    }
    else                    // first process
    {
        if (close(wfd) == -1)
            err("primes: close() fail\n");

        int status;
        while (wait(&status) != -1) ;
    }

    // exit normally
    exit(0);
}

void test_prime(int num)
{
    // if can't tell num is not a prime, pass it to next process
    if (num % prime != 0)
    {
        // when no next, build a pipe and fork a next
        if (!next)
        {
            if (pipe(pfd) == -1)
                err("primes: pipe() fail\n");

            next = fork();
            if (next == -1)         // error
                err("primes: fork() fail\n");
            else if (next == 0)     // child
            {
                rfd = pfd[0];
                if (close(pfd[1]) == -1)
                    err("primes: close() fail\n");
                if (read(rfd, &prime, sizeof(prime)) != sizeof(prime))
                    err("primes: child read() fail\n");
                printf("prime %d\n", prime);
                return;
            }
            else                    // parent
            {
                wfd = pfd[1];
                if (close(pfd[0]) == -1)
                    err("primes: close() fail\n");
            }
        }
        
        // send the num to next
        if (write(wfd, &num, sizeof(num)) == -1)
            err("primes: write() fail\n");
    }
}