//
// Created by tsaran on 07.05.24.
//

// this represents work that has to be
// completed by a thread in the pool

typedef struct {
    void (*function)(void *p);
    void *data;
} task;

struct node {
    task *t;
    struct node *next;
};

void insert(struct node **head, task *t);
void delete(struct node **head, task *t);
void traverse(struct node *head);