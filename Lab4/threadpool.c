/**
 * Implementation of thread pool.
 */

#include "list.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "threadpool.h"


#define QUEUE_SIZE 10
#define NUMBER_OF_THREADS 3

#define TRUE 1




// the work queue
task worktodo;

// the worker bee
pthread_t bees[NUMBER_OF_THREADS];
pthread_mutex_t queue_lock = PTHREAD_MUTEX_INITIALIZER;
sem_t sem;

struct node *queue = NULL;

// insert a task into the queue
// returns 0 if successful or 1 otherwise, 
int enqueue(task t) 
{
    task *new_task = malloc(sizeof(task));
    if (!new_task)
        return 1;

    new_task->function = t.function;
    new_task->data = t.data;

    pthread_mutex_lock(&queue_lock);
    insert(&queue, new_task);
    pthread_mutex_unlock(&queue_lock);

    return 0;
}

// remove a task from the queue
task dequeue() 
{
    pthread_mutex_lock(&queue_lock);
    delete(&queue, queue->t);
    pthread_mutex_unlock(&queue_lock);
    return worktodo;
}

// the worker thread in the thread pool
void *worker(void *param) {
    while (queue != NULL) {
        sem_wait(&sem);
        execute(queue->t->function, queue->t->data);
        dequeue();
    }
    pthread_exit(0);
}

/**
 * Executes the task provided to the thread pool
 */
void execute(void (*somefunction)(void *p), void *p)
{
    (*somefunction)(p);
}

/**
 * Submits work to the pool.
 */
int pool_submit(void (*somefunction)(void *p), void *p)
{
    int enqueue_result = 0;

    worktodo.function = somefunction;
    worktodo.data = p;

    enqueue_result = enqueue(worktodo);

    if (!enqueue_result)
        sem_post(&sem);

    return enqueue_result;
}

// initialize the thread pool
void pool_init(void)
{
    pthread_mutex_init(&queue_lock, NULL);
    sem_init(&sem, 0, 0);
    for (int th = 0; th < NUMBER_OF_THREADS; ++th)
        pthread_create(&bees[th],NULL,worker,NULL);
}

// shutdown the thread pool
void pool_shutdown(void)
{
    pthread_mutex_destroy(&queue_lock);
    sem_destroy(&sem);
    for (int th = 0; th < NUMBER_OF_THREADS; ++th) {
        pthread_cancel(bees[th]);
        pthread_join(bees[th], NULL);
    }
}
