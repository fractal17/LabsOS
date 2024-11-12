

#include "stdio.h"
#include "stdlib.h"
#include "task.h"
#include "list.h"
#include "cpu.h"


struct node *head = NULL;

void add(char *name, int priority, int burst) {
    Task *task = malloc(sizeof(task));
    task->name = name;
    task->priority = priority;
    task->burst = burst;

    insert(&head, task);
}

Task *pick_task() {
    if (head == NULL)
        return NULL;

    struct node *curr = head;
    struct node *prev = NULL;

    while (curr->next != NULL) {
        prev = curr;
        curr = curr->next;
    }

    Task *last_task = curr->task;

    if (prev == NULL)
        delete(&head, head->task);

    else
        delete(&head, last_task);

    return last_task;
}

void schedule(Metrics *metrics) {
    Task *task = pick_task();
    int curr_time = 0;

    while (task != NULL) {
        metrics->response_time += curr_time;

        run(task, task->burst);

        curr_time += task->burst;
        metrics->turnaround_time += curr_time;

        task = pick_task();
    }
}