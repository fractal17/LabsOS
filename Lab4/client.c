/**
 * Example client program that uses thread pool.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "threadpool.h"

struct data
{
    int a;
    int b;
};

void add(void *param)
{
    struct data *temp;
    temp = (struct data*)param;

    printf("I add two values %d and %d result = %d\n",temp->a, temp->b, temp->a + temp->b);
}

int main(void)
{
    srand(time(NULL));

    // create some work to do
    struct data works[10];
    for (int i = 0; i < sizeof(works) / sizeof(struct data); ++i) {
        works[i].a = rand() % 101;
        works[i].b = rand() % 101;
    }

    // initialize the thread pool
    pool_init();

    // submit the work to the queue
    for (int i = 0; i < sizeof(works) / sizeof(struct data); ++i) {
        pool_submit(&add, &works[i]);
    }

    // may be helpful 
    sleep(1);

    pool_shutdown();

    return 0;
}
