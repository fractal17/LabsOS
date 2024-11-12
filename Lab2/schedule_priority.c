
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

    Task *max_priority_task = head->task;
    struct node *curr = head->next;

    while (curr != NULL) {
        if (curr->task->priority > max_priority_task->priority)
            max_priority_task = curr->task;

        curr = curr->next;
    }

    delete(&head, max_priority_task);

    return max_priority_task;
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

