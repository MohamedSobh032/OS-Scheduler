/**
 * @file PrioQueue.h
 * @brief Header file for Priority Queue.
 */
#ifndef _PRIO_QUEUE_H_
#define _PRIO_QUEUE_H_

#include "../headers.h"

/**
 * @brief Structure representing a node in the priority queue.
 */
typedef struct Prio_Node {
  int prio;               /**< Priority of the process */
  struct PCB process;     /**< Process Control Block */
  struct Prio_Node* next; /**< Pointer to the next node */
} Prio_Node;

/**
 * @brief Structure representing a priority queue.
 */
typedef struct Prio_Queue {
  struct Prio_Node* head; /**< Pointer to the head of the queue */
  struct Prio_Node* tail; /**< Pointer to the tail of the queue */
} Prio_Queue;

/**
 * @brief Initializes a priority queue.
 *
 * @param q Pointer to the priority queue to be initialized.
 */
void Prio_Queue_Init(Prio_Queue* q) {
  q->head = NULL;
  q->tail = NULL;
}

/**
 * @brief Enqueues a process into the priority queue based on its priority.
 *
 * @param q Pointer to the priority queue.
 * @param prio Priority of the process to be enqueued.
 * @param process Process Control Block (PCB) to be enqueued.
 */
void Prio_Queue_enqueue(Prio_Queue* q, int prio, PCB process) {
  // Create new node
  struct Prio_Node* newNode = (Prio_Node*)malloc(sizeof(Prio_Node));
  if (newNode == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    return;
  }
  newNode->prio = prio;
  newNode->process = process;
  newNode->next = NULL;
  // case queue is empty
  if (q->head == NULL) {
    q->head = newNode;
    q->tail = newNode;
  } else {
    struct Prio_Node* current = q->head;
    struct Prio_Node* prev = NULL;
    // Traverse the queue to find the correct position based on priority
    while (current != NULL && current->prio <= prio) {
      prev = current;
      current = current->next;
    }
    if (prev == NULL) {
      newNode->next = q->head;
      q->head = newNode;
    } else {
      prev->next = newNode;
      newNode->next = current;
    }
    // Update tail if newNode is the last node
    if (newNode->next == NULL) {
      q->tail = newNode;
    }
  }
}

/**
 * @brief Dequeues the highest priority process from the priority queue.
 *
 * @param q Pointer to the priority queue.
 * @return PCB of the dequeued process.
 */
PCB Prio_Queue_dequeue(Prio_Queue* q) {
  struct PCB emptyPCB;
  // Check if the queue is empty
  if (q->head == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    return emptyPCB;
  }

  // Remove the node from the head of the queue
  struct Prio_Node* temp = q->head;
  emptyPCB = temp->process;
  q->head = q->head->next;

  // If the queue is now empty, update tail to NULL
  free(temp);
  if (q->head == NULL) {
    q->tail = NULL;
  }

  return emptyPCB;
}

/**
 * @brief Checks if the priority queue is empty.
 *
 * @param q Pointer to the priority queue.
 * @return true if the priority queue is empty, false otherwise.
 */
bool Prio_Queue_isEmpty(Prio_Queue* q) { return (bool)(q->head == NULL); }

#endif /* _PRIO_QUEUE_H_ */