/**
 * Various list operations
 */

#include "list.h"
#include <stdlib.h>


void insert(struct node **head, task *new_task) {
    struct node *new_node = malloc(sizeof(struct node));

    new_node->t = new_task;
    new_node->next = *head;
    *head = new_node;
}

void delete(struct node **head, task *task) {
    struct node *temp;
    struct node *prev;

    temp = *head;

    if (temp->t == task)
        *head = (*head)->next;

    else {
        prev = *head;
        temp = temp->next;
        while (temp->t != task) {
            prev = temp;
            temp = temp->next;
        }
        prev->next = temp->next;
    }
}