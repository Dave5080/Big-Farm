#ifndef SIMPLE_QUEUE_H
#define SIMPLE_QUEUE_H

#include "xerrori.h"



struct Node {
	void *value;
	struct Node *next;
};

struct Queue {
	int size;
	int max_size;
	struct Node *head;
	struct Node *tail;
    sem_t sem_mutex, sem_full, sem_empty;
};

struct Queue* newQueue(int capacity);
int enqueue(struct Queue *q, void *value);
void* dequeue(struct Queue *q);
void freeQueue(struct Queue *q);

#endif