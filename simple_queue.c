#include "simple_queue.h"


struct Queue* newQueue(int capacity)
{
	struct Queue *q;
	q = malloc(sizeof(struct Queue));

	if (q == NULL) {
		return q;
	}

	q->size = 0;
	q->max_size = capacity;
	q->head = NULL;
	q->tail = NULL;

	xsem_init(&(q->sem_full), 0, 0, QUI);
	xsem_init(&(q->sem_mutex), 0, 1, QUI);
	xsem_init(&(q->sem_empty), 0, capacity, QUI);

	return q;
}

int enqueue(struct Queue *q, void *value)
{	
	xsem_wait(&(q->sem_empty), QUI);
	xsem_wait(&(q->sem_mutex), QUI);
	
	if ((q->size + 1) > q->max_size) {
		return q->size;
	}

	struct Node *node = malloc(sizeof(struct Node));

	if (node == NULL) {
		return q->size;
	}

	node->value = value;
	node->next = NULL;

	if (q->head == NULL) {
		q->head = node;
		q->tail = node;
		q->size = 1;

		xsem_post(&(q->sem_mutex), QUI);
		xsem_post(&(q->sem_full), QUI);

		return q->size;
	}


	q->tail->next = node;
	q->tail = node;
	q->size += 1;
	int newsize = q->size;
	
	xsem_post(&(q->sem_mutex), QUI);
	xsem_post(&(q->sem_full), QUI);
	return q->size;
}

void* dequeue(struct Queue *q)
{	
	xsem_wait(&(q->sem_full), QUI);
	xsem_wait(&(q->sem_mutex), QUI);
	if (q->size == 0) {

		xsem_post(&(q->sem_mutex), QUI);
		xsem_post(&(q->sem_empty), QUI);

		return NULL;
	}

	void *value = NULL;
	struct Node *tmp = NULL;

	value = q->head->value;
	tmp = q->head;
	q->head = q->head->next;
	q->size -= 1;

	free(tmp);

	xsem_post(&(q->sem_mutex), QUI);
	xsem_post(&(q->sem_empty), QUI);
	return value;
}

void freeQueue(struct Queue *q)
{
	if (q == NULL) {
		return;
	}

	while (q->head != NULL) {
		struct Node *tmp = q->head;
		q->head = q->head->next;
		if (tmp->value != NULL) {
			free(tmp->value);
		}

		free(tmp);
	}

	if (q->tail != NULL) {
		free(q->tail);
	}

	free (q);
}