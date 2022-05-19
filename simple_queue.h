#include <stdlib.h>

struct Node {
	void *value;
	struct Node *next;
};

struct Queue {
	int size;
	int max_size;
	struct Node *head;
	struct Node *tail;
};

struct Queue* newQueue(int capacity);
int enqueue(struct Queue *q, void *value);
void* dequeue(struct Queue *q);
void freeQueue(struct Queue *q);