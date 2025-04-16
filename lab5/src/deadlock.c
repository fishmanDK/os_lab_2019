#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t lock1, lock2;

void *resource1(void *arg)
{
    pthread_mutex_lock(&lock1);
    printf("Thread 1: захватил lock1\n");
    sleep(1);

    printf("Thread 1: пытается захватить lock2...\n");
    pthread_mutex_lock(&lock2);
    printf("Thread 1: захватил lock2\n");

    pthread_mutex_unlock(&lock2);
    pthread_mutex_unlock(&lock1);
    pthread_exit(NULL);
}

void *resource2(void *arg)
{
    pthread_mutex_lock(&lock2);
    printf("Thread 2: захватил lock2\n");
    sleep(1);

    printf("Thread 2: пытается захватить lock1...\n");
    pthread_mutex_lock(&lock1);
    printf("Thread 2: захватил lock1\n");

    pthread_mutex_unlock(&lock1);
    pthread_mutex_unlock(&lock2);
    pthread_exit(NULL);
}

int main() 
{
    pthread_mutex_init(&lock1, NULL);
    pthread_mutex_init(&lock2, NULL);
    pthread_t thread1, thread2;

    pthread_create(&thread1, NULL, resource1, NULL);
    pthread_create(&thread2, NULL, resource2, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    pthread_mutex_destroy(&lock1);
    pthread_mutex_destroy(&lock2);
    return 0;
}