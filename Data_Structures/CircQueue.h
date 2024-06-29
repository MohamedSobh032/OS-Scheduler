/**
 * @file CircQueue.h
 * @brief Header file for Circular Queue.
 */
#ifndef _CIRC_QUEUE_H_
#define _CIRC_QUEUE_H_

/**
 * @brief Structure representing a node in the circular queue.
 */
typedef struct Circ_Node {
  struct PCB process;     /**< Process Control Block */
  struct Circ_Node* next; /**< Pointer to the next node */
} Circ_Node;

/**
 * @brief Structure representing a circular queue.
 */
typedef struct Circ_Queue {
  struct Circ_Node* head; /**< Pointer to the head of the queue */
  struct Circ_Node* tail; /**< Pointer to the tail of the queue */
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
  struct Circ_Node* newNode = (Circ_Node*)malloc(sizeof(Circ_Node));
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
    newNode->next = newNode;
  } else {
    newNode->next = q->head;
    q->tail->next = newNode;
    q->tail = newNode;
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
    return emptyPCB;
  }
  // Remove the node from the head of the queue
  struct Circ_Node* temp = q->head;
  emptyPCB = temp->process;
  if (q->head == q->tail) {
    q->head = NULL;
    q->tail = NULL;
  } else {
    q->head = q->head->next;
    q->tail->next = q->head;
  }
  free(temp);
  return emptyPCB;
}

/**
 * @brief Checks if the circular queue is empty.
 *
 * @param q Pointer to the circular queue.
 * @return true if the circular queue is empty, false otherwise.
 */
bool Circ_Queue_isEmpty(Circ_Queue* q) { return (bool)(q->head == NULL); }

/**
 * @brief Increments the waiting time of all process in the circular queue.
 *
 * @param q Pointer to the circular queue.
 * @param time Current time.
 */
void Circ_Queue_Inc_WaitingTime(Circ_Queue* q, int time) {
  if (q->head == NULL) {
    return;
  }
  struct Circ_Node* current = q->head;
  while (current != q->tail) {
    if (current->process.arrivalTime < time) {
      current->process.waitTime++;
    }
    current = current->next;
  }
  if (current->process.arrivalTime < time) {
    current->process.waitTime++;
  }
}

#endif /* _CIRC_QUEUE_H_ */