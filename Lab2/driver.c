/**
 * Driver.c
 *
 * Schedule is in the format
 *
 *  [name] [priority] [CPU burst]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "list.h"
#include "schedulers.h"

#define SIZE    100

int main(int argc, char *argv[])
{
    FILE *in;
    char *temp;
    char task[SIZE];

    char *name;
    int priority;
    int burst;

    int num_of_tasks = 0;

    in = fopen(argv[1],"r");
    
    while (fgets(task,SIZE,in) != NULL) {
        temp = strdup(task);
        name = strsep(&temp,",");
        priority = atoi(strsep(&temp,","));
        burst = atoi(strsep(&temp,","));

        // add the task to the scheduler's list of tasks
        add(name,priority,burst);

        ++num_of_tasks;

        free(temp);
    }

    fclose(in);

    Metrics metrics = {0, 0, 0};

    // invoke the scheduler
    schedule(&metrics);

    double turnaround_time = (double)metrics.turnaround_time / num_of_tasks;
    double waiting_time = (double)metrics.waiting_time / num_of_tasks;
    double response_time = (double)metrics.response_time / num_of_tasks;

    printf("Turnaround time: %.4f\n", turnaround_time);
    printf("Waiting time: %.4f\n", waiting_time);
    printf("Response time: %.4f\n", response_time);

    return 0;
}
