#define _POSIX_C_SOURCE 200809L
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#define CAPACITY 5
#define ITEMS 20
static int buffer[CAPACITY],in=0,out=0;
static sem_t empty_slots,full_slots;
static pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
static long consumed_sum=0;
static int consumed_count=0;

static void *producer(void *arg){
    (void)arg;
    for(int item=1;item<=ITEMS;item++){
        sem_wait(&empty_slots);
        pthread_mutex_lock(&mutex);
        buffer[in]=item;in=(in+1)%CAPACITY;
        printf("Producer: produced item %d\n",item);
        pthread_mutex_unlock(&mutex);
        sem_post(&full_slots);
    }
    return NULL;
}
static void *consumer(void *arg){
    (void)arg;
    for(int i=0;i<ITEMS;i++){
        sem_wait(&full_slots);
        pthread_mutex_lock(&mutex);
        int item=buffer[out];out=(out+1)%CAPACITY;
        consumed_sum+=item;consumed_count++;
        printf("Consumer: consumed item %d\n",item);
        pthread_mutex_unlock(&mutex);
        sem_post(&empty_slots);
    }
    return NULL;
}
int main(void){
    pthread_t p,c;
    puts("OPERATING SYSTEMS - ASSIGNMENT 2");
    puts("Student Name: V. Sriram | Register No: 192524037 | AI & DS");
    sem_init(&empty_slots,0,CAPACITY);sem_init(&full_slots,0,0);
    if(pthread_create(&p,NULL,producer,NULL)||pthread_create(&c,NULL,consumer,NULL)){perror("pthread_create");return 1;}
    pthread_join(p,NULL);pthread_join(c,NULL);
    printf("items consumed: %d, sum = %ld (expected 210)\n",consumed_count,consumed_sum);
    sem_destroy(&empty_slots);sem_destroy(&full_slots);pthread_mutex_destroy(&mutex);
    return consumed_sum==210 && consumed_count==ITEMS ? 0:1;
}
