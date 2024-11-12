#include "stdio.h"
#include "stdlib.h"
#include "task.h"
#include "list.h"
#include "cpu.h"

#define QUANT 10

struct node *head = NULL;
struct node *temp = NULL;

void add(char *name, int priority, int burst) {
    Task *task = malloc(sizeof(task));
    task->name = name;
    task->priority = priority;
    task->burst = burst;

    insert(&head, task);
}

Task *pick_task() {
    if (temp == NULL)
        return NULL;

    Task *task = temp->task;
    temp = temp->next;

    return task;
}

void schedule(Metrics *metrics) {
    temp = head;
    int is_first_start = 1;
    int temp_turnaround_time = 0;

    while (head != NULL) {
        Task *task = pick_task();
        int curr_time = 0;

        while (task != NULL) {
            if (is_first_start)
                metrics->response_time += curr_time;

            if (QUANT < task->burst) {
                run(task, QUANT);
                task->burst -= QUANT;

                curr_time += QUANT;
                metrics->turnaround_time += curr_time;
                metrics->waiting_time += curr_time - QUANT;

            }
            else {
                run(task, task->burst);
                delete(&head, task);

                curr_time += task->burst;
                metrics->turnaround_time += curr_time;
                metrics->waiting_time += curr_time - task->burst;
            }
            task = pick_task();
        }
        temp = head;

        if (is_first_start) {
            temp_turnaround_time = metrics->turnaround_time;
            is_first_start = 0;
        }
    }
    metrics->turnaround_time += temp_turnaround_time;
}