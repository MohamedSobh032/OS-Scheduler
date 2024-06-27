/**
 * @file CircQueue.h
 * @brief Header file for Circular Queue.
 */
#ifndef _CIRC_QUEUE_H_
#define _CIRC_QUEUE_H_

/**
 * @brief Structure representing a node in the circular queue.
 */
typedef struct _Node {
  struct PCB process; /**< Process Control Block */
  struct _Node* next; /**< Pointer to the next node */
} _Node;

/**
 * @brief Structure representing a circular queue.
 */
typedef struct Circ_Queue {
  struct _Node* head; /**< Pointer to the head of the queue */
  struct _Node* tail; /**< Pointer to the tail of the queue */
} Circ_Queue;

/**
 * @brief Initializes a circular queue.
 *
 * @param q Pointer to the circular queue to be initialized.
 */
void Circ_Queue_Init(Circ_Queue* q) {
  q->head = NULL;
  q->tail = NULL;
}

/**
 * @brief Enqueues a process into the circular queue.
 *
 * @param q Pointer to the circular queue.
 * @param process Process Control Block (PCB) to be enqueued.
 */
void Circ_Queue_enqueue(Circ_Queue* q, PCB process) {
  // Create new node
  struct _Node* newNode = (_Node*)malloc(sizeof(_Node));
  if (newNode == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    return;
  }
  newNode->process = process;
  // case queue is empty
  if (q->head == NULL) {
    newNode->next = newNode;
    q->head = newNode;
    q->tail = newNode;
  } else {
    newNode->next = q->head;
    q->tail->next = newNode;
  }
}

/**
 * @brief Dequeues the head from the circular queue.
 *
 * @param q Pointer to the circular queue.
 * @return PCB of the dequeued process.
 */
PCB Circ_Queue_dequeue(Circ_Queue* q) {
  struct PCB emptyPCB;
  emptyPCB.id = -1;
  // Check if the queue is empty
  if (q->head == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    return emptyPCB;
  }

  // Remove the node from the head of the queue
  struct _Node* temp = q->head;
  emptyPCB = temp->process;
  q->head = q->head->next;

  // If the queue is now empty, update tail to NULL
  free(temp);
  if (q->head == NULL) {
    q->tail = NULL;
  } else {
    q->tail->next = q->head;
  }

  return emptyPCB;
}

/**
 * @brief Checks if the circular queue is empty.
 *
 * @param q Pointer to the circular queue.
 * @return true if the circular queue is empty, false otherwise.
 */
bool Circ_Queue_isEmpty(Circ_Queue* q) { return (bool)(q->head == NULL); }

#endif /* _CIRC_QUEUE_H_ */