#define _POSIX_C_SOURCE 200809L
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#define J 64
long out[J];
void*work(void*x){long id=(long)x,s=0;for(long k=0;k<250000;k++)s=(s+(k^(id+17)))%1000003;out[id]=s;return NULL;}
int main(void){int ns[]={1,4,8,16,64};for(int z=0;z<5;z++){int n=ns[z];pthread_t th[J];struct timespec a,b;clock_gettime(CLOCK_MONOTONIC,&a);for(long i=0;i<n;i++)pthread_create(&th[i],0,work,(void*)i);for(int i=0;i<n;i++)pthread_join(th[i],0);clock_gettime(CLOCK_MONOTONIC,&b);printf("%d threads: %.6f s\n",n,(b.tv_sec-a.tv_sec)+(b.tv_nsec-a.tv_nsec)/1e9);}return 0;}