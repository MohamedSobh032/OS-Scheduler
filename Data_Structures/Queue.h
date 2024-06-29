/**
 * @file Queue.h
 * @brief Header file for Queue.
 */

#ifndef _QUEUE_H_
#define _QUEUE_H_

/**
 * @brief Structure representing a node in the queue.
 */
typedef struct _Node {
  struct PCB process; /**< Process Control Block */
  struct _Node* next; /**< Pointer to the next node */
} _Node;

/**
 * @brief Structure representing a queue.
 */
typedef struct _Queue {
  struct _Node* head; /**< Pointer to the head of the queue */
  struct _Node* tail; /**< Pointer to the tail of the queue */
} _Queue;

/**
 * @brief Initializes a queue.
 *
 * @param q Pointer to the queue to be initialized.
 */
void _Queue_Init(_Queue* q) {
  q->head = NULL;
  q->tail = NULL;
}

/**
 * @brief Enqueues a process into the queue.
 *
 * @param q Pointer to the queue.
 * @param process Process Control Block (PCB) to be enqueued.
 */
void _Queue_enqueue(_Queue* q, PCB process) {
  // Create new node
  struct _Node* newNode = (_Node*)malloc(sizeof(_Node));
  if (newNode == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    return;
  }
  newNode->process = process;
  newNode->next = NULL;
  // case queue is empty
  if (q->head == NULL) {
    q->head = newNode;
    q->tail = newNode;
  } else {
    q->tail->next = newNode;
    q->tail = newNode;
  }
}

/**
 * @brief Dequeues the head from the queue.
 *
 * @param q Pointer to the queue.
 * @return PCB of the dequeued process.
 */
PCB _Queue_dequeue(_Queue* q) {
  struct PCB emptyPCB;
  emptyPCB.id = -1;
  // Check if the queue is empty
  if (q->head == NULL) {
    return emptyPCB;
  }
  // Remove the node from the head of the queue
  struct _Node* temp = q->head;
  emptyPCB = temp->process;

  q->head = q->head->next;

  free(temp);
  return emptyPCB;
}

/**
 * @brief Checks if the queue is empty.
 *
 * @param q Pointer to the queue.
 * @return true if the queue is empty, false otherwise.
 */
bool _Queue_isEmpty(_Queue* q) { return (bool)(q->head == NULL); }

#endif /* _QUEUE_H_ */