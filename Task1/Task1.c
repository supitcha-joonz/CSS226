#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

void printFibo(int n, int *fibo)
{
    int i;
    for(i=0; i<=n; i++)
        printf("%d -> %d\n" ,i, fibo[i]);
}

void computeFibo(int n, int *fibo) 
{
    int i;
    fibo[0] = 0;
    fibo[1] = 1;

    for (i=2; i<=n; i++) 
        fibo[i] = fibo[i-1] + fibo[i-2];
}

int main(int argc, char *argv[])
{
    pid_t childPID;
    int status;
    int shm_fd;
    int* shared_memory;
    int msize; // the size (in bytes) of the shared memory segment 
    const char *name = "FIBONACCI_SERIES";
    int n;

    if (argc!=2) 
    {
        fprintf(stderr, "usage: %s <Fibonacci number to be generated>\n", argv[0]);
        return -1;
    }

    n = atoi(argv[1]);
    if (n < 0) 
    {
        fprintf(stderr, "Illegal fibonacci number: %s\n", argv[1]);
        return -2;
    }

    // calculating the array size based on the number of terms being passed from child to parent
    msize = (n+2)*sizeof(int); 

    // open the memory
    shm_fd = shm_open (name, O_CREAT | O_EXCL | O_RDWR, S_IRWXU | S_IRWXG);
    if (shm_fd < 0) 
    {
        fprintf(stderr,"Error in shm_open()");
        return -3;
    }

    printf("Created shared memory object %s\n", name);

    // attach the shared memory segment
    ftruncate(shm_fd, msize);
    printf("shmat returned\n");

    // allocating the shared memory
    shared_memory = (int *) mmap(NULL, msize, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_memory == NULL) 
    {
        fprintf(stderr,"Error in mmap()");
        return -3;
    }

    printf("Shared memory segment allocated correctly (%d bytes).\n", msize);

    shared_memory[0] = n;

    childPID=fork();
    if ( childPID == -1 ) 
    {
        fprintf(stderr, "Cannot proceed. fork() error");
        return -4;
    }
    if (childPID  == 0) 
    {
        // then we're the child process
        computeFibo(shared_memory[0],shared_memory+1);
        exit(0);
    }
    else
    {
        // parent will wait until the child finished
        wait(&status);

        // print the final results in the 
        printFibo(shared_memory[0], shared_memory+1);

        // now detach the shared memory segment
        shm_unlink(name);
    }
    return 0;
}
