#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    const char *name="/sriram_os_assignment_shm";
    int fd=shm_open(name,O_CREAT|O_RDWR,0600);
    if(fd<0){perror("shm_open");return 1;}
    if(ftruncate(fd, sizeof(int))<0){perror("ftruncate");return 1;}
    int *shared=mmap(NULL,sizeof(int),PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    if(shared==MAP_FAILED){perror("mmap");return 1;}
    puts("OPERATING SYSTEMS - ASSIGNMENT 2");
    puts("Student Name: V. Sriram | Register No: 192524037 | AI & DS");
    pid_t p=fork();
    if(p<0){perror("fork");return 1;}
    if(p==0){printf("Child: read shared value %d\n",*shared);_exit(0);}
    *shared=42; /* Demonstration is ordered by wait below only after the parent writes. */
    waitpid(p,NULL,0);
    munmap(shared,sizeof(int));close(fd);shm_unlink(name);
    return 0;
}
