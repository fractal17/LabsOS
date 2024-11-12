#include "stdio.h"
#include "stdlib.h"
#include "task.h"
#include "list.h"
#include "cpu.h"

#define QUANT 10

struct node *head = NULL;
struct node *temp = NULL;

struct node *not_prioritized_task_list = NULL;
struct node *prioritized_task_list = NULL;

int priority_task_count[10] = {0};

void add(char *name, int priority, int burst) {
    Task *task = malloc(sizeof(task));
    task->name = name;
    task->priority = priority;
    task->burst = burst;

    insert(&head, task);
}

Task *pick_non_priority_task() {
    if (not_prioritized_task_list == NULL)
        return NULL;

    Task *min_priority_task = not_prioritized_task_list->task;
    struct node *curr = not_prioritized_task_list->next;

    while (curr != NULL) {
        if (curr->task->priority < min_priority_task->priority)
            min_priority_task = curr->task;

        curr = curr->next;
    }

    delete(&not_prioritized_task_list, min_priority_task);

    return min_priority_task;
}

Task *pick_task() {
    if (temp == NULL)
        return NULL;

    Task *task = temp->task;
    temp = temp->next;

    return task;
}

void schedule_rr(Metrics *metrics, int *time) {
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

                curr_time += QUANT; time += QUANT;
                metrics->turnaround_time += curr_time;
                metrics->waiting_time += curr_time - QUANT;

            }
            else {
                run(task, task->burst);
                delete(&head, task);

                curr_time += task->burst; time += task->burst;
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

void schedule(Metrics *metrics) {
    not_prioritized_task_list = head;
    prioritized_task_list = NULL;

    Task * priority_task;
    while ((priority_task = pick_non_priority_task()) != NULL) {
        insert(&prioritized_task_list, priority_task);
        ++priority_task_count[priority_task->priority];
    }

    int curr_time = 0;

    while (prioritized_task_list != NULL) {
        Task *task = prioritized_task_list->task;

        if (priority_task_count[task->priority] > 1) {
            struct node *temp_tasks_list = NULL;

            while (priority_task_count[task->priority] != 0) {
                Task *temp_task = prioritized_task_list->task;
                insert(&temp_tasks_list, temp_task);

                --priority_task_count[task->priority];
                prioritized_task_list = prioritized_task_list->next;
            }
            head = temp_tasks_list;
            schedule_rr(metrics, &curr_time);
        }
        else {
            metrics->response_time += curr_time;
            
            run(task, task->burst);
            prioritized_task_list = prioritized_task_list->next;
            
            curr_time += task->burst;
            metrics->turnaround_time += curr_time;
            metrics->waiting_time += curr_time - task->burst;
        }
    }
}
