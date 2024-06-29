#include "headers.h"

/******************** MACROS ********************/
#define __PROCESS_NUMBER_ID__ 1
#define __ALGORITHM_NUMBER_ID__ 2
#define __QUANTUM_SIZE_ID__ 3
/************************************************/

/*************** Global Variables ***************/
static int processNumber;       // NOLINT
static int algo;                // NOLINT
static int quantumSize;         // NOLINT
static int msg_id;              // NOLINT
static ssize_t rec_val;         // NOLINT
static struct msgbuff message;  // NOLINT
static int receivedProcesses;   // NOLINT
/************************************************/

/************* Function Definitions *************/
struct PCB rec_msg_queue(void);
void HPF(void);
void SRTN(void);
void RR(void);
/************************************************/

int main(int argc, char* argv[]) {
  /****************************** Initialization ******************************/
  initClk();
  /* Initialize the message queue */
  key_t key_id = ftok(__FILE_KEY_NAME__, __FILE_KEY_VAL__);
  msg_id = msgget(key_id, 0666 | IPC_CREAT);
  if (msg_id == -1) {
    perror("Error in create");
    exit(-1);
  }
  /* Initialize the global variables */
  processNumber = atoi(argv[__PROCESS_NUMBER_ID__]);
  algo = atoi(argv[__ALGORITHM_NUMBER_ID__]);
  quantumSize = atoi(argv[__QUANTUM_SIZE_ID__]);
  initializeBuddyAllocator();
  /****************************************************************************/

  /**************************** Algorithm Choosing ****************************/
  if (algo == 0) {
    /* Highest Priority First */
    HPF();
  } else if (algo == 1) {
    /* Shortest Remaining Time Next */
    SRTN();
  } else if (algo == 2) {
    /* Round Robin */
    RR();
  }
  /****************************************************************************/

  // upon termination release the clock resources.
  destroyClk(true);
}

/**
 * @brief Receive a process message from a message queue.
 *
 * This function attempts to receive a process message from a message queue
 * identified by `msg_id`. If a message is received successfully, it initializes
 * a PCB structure with the received process's parameters and increments the
 * count of received processes.
 *
 * @details
 * - Uses `msgrcv` to receive a message from the message queue.
 * - Initializes a PCB structure with process parameters upon successful
 * receipt.
 * - Handles errors during the message reception process, including checking for
 *   the absence of messages (ENOMSG).
 *
 * @note
 * - Assumes the existence of a message queue identified by `msg_id`.
 * - Assumes a message structure (`message`) containing process parameters.
 * - Sets `pcb.id` to `-1` if no message is received or an error occurs.
 *
 * @return struct PCB The PCB structure initialized with received process
 * parameters, if no received process, returns a process with id = -1.
 */
struct PCB rec_msg_queue(void) {
  /* Create the returned structure */
  struct PCB pcb;
  pcb.id = -1;
  rec_val = msgrcv(msg_id, &message, sizeof(message.process), __MSG_TYPE__,
                   IPC_NOWAIT);
  if (rec_val == -1) {
    if (errno != ENOMSG) {
      perror("Error in receiving process");
      exit(-1);
    }
  } else {
    /* Recieve the new process and initialize its parameters */
    pcb.id = message.process.id;
    pcb.arrivalTime = message.process.arrivalTime;
    pcb.runTime = message.process.runTime;
    pcb.prio = message.process.prio;
    pcb.memory = message.process.memory;
    /* Set unreceived data */
    pcb.remainingTime = message.process.runTime;
    pcb.state = _NEW;
    pcb.waitTime = 0;
    /* Increment received process number */
    receivedProcesses++;
  }
  return pcb;
}

/**
 * @brief Highest Priority First (HPF) scheduling algorithm.
 *
 * This function implements the HPF scheduling algorithm using a priority queue.
 * It receives processes from a message queue, prioritizes them based on their
 * priority, and executes them accordingly using fork and exec.
 *
 * @details
 * - Initializes necessary variables and structures.
 * - Continuously receives processes until all processes are received and
 *   the priority queue is empty.
 * - Iterates in time steps (each second) and handles process forking,
 *   execution, and termination.
 *
 * @note
 * - Uses functions from Prio_Queue module for queue operations.
 * - Uses getClk() to get current clock time.
 * - Uses kill() to terminate processes.
 *
 * @warning
 * - Uses SIGKILL to forcefully terminate processes.
 *
 * @param None
 * @return None
 */
void HPF(void) {
  /****************************** Initialization ******************************/
  int oldClk = getClk();  /**< Clock counter */
  struct PCB rec;         /**< PCB to receive processes */
  struct Prio_Queue q;    /**< Priority queue to implement the algorithm */
  struct PCB process;     /**< Process to be currently executed */
  bool currently = false; /**< Currently running a process */
  Prio_Queue_Init(&q);    /**< Initialize the priority queue */
  /****************************************************************************/

  while ((receivedProcesses < processNumber) || !Prio_Queue_isEmpty(&q) ||
         (currently == true)) {
    /***************************** Receive Process ****************************/
    rec = rec_msg_queue();
    if (rec.id != -1) {
      Prio_Queue_enqueue(&q, rec.prio, rec);
      /* Print Statement */
      printf("At time = %d, received process with ID = %d\n", getClk(), rec.id);
      if (currently == false) {
        /* Dequeue the head of the queue (highest priority process) */
        process = Prio_Queue_dequeue(&q);
        if (process.id != -1) {
          /* Fork new process */
          int process_id = fork();
          if (process_id == -1) {
            perror("Error in forking of a process ");
            exit(-1);
          } else if (process_id == 0) {  // Child
            execl("./process.out", "process.out", NULL);
          } else {  // Parent
            currently = true;
            process.startTime = oldClk;
            process.PID = process_id;
            process.state = _RUNNING;
            printf("At time = %d, new process with ID = %d started running\n",
                   oldClk, process.id);
          }
        }
      }
    }
    /**************************************************************************/

    /******************************** TIME STEP *******************************/
    if ((getClk() - oldClk) == 1) {
      Prio_Queue_Inc_WaitingTime(&q, oldClk);
      oldClk = getClk();

      /************************** NEW PROCESS FORKING *************************/
      if (currently == false) {
        /* Dequeue the head of the queue (highest priority process) */
        process = Prio_Queue_dequeue(&q);
        if (process.id != -1) {
          /* Fork new process */
          int process_id = fork();
          if (process_id == -1) {
            perror("Error in forking of a process ");
            exit(-1);
          } else if (process_id == 0) {  // Child
            execl("./process.out", "process.out", NULL);
          } else {  // Parent
            currently = true;
            process.startTime = oldClk;
            process.PID = process_id;
            process.state = _RUNNING;
            printf("At time = %d, new process with ID = %d started running\n",
                   oldClk, process.id);
          }
        }
      }
      /************************************************************************/

      /*************************** NORMAL PROCESSING **************************/
      else {
        process.remainingTime--;
        /* Destroy the running process */
        if (process.remainingTime == 0) {
          currently = false;
          process.endTime = oldClk;
          kill(process.PID, SIGKILL);
          /* Print statement */
          printf("At time = %d, process with ID = %d, has finished\n", oldClk,
                 process.id);
          /* Start new process */
          /* Dequeue the head of the queue (highest priority process) */
          process = Prio_Queue_dequeue(&q);
          if (process.id != -1) {
            /* Fork new process */
            int process_id = fork();
            if (process_id == -1) {
              perror("Error in forking of a process ");
              exit(-1);
            } else if (process_id == 0) {  // Child
              execl("./process.out", "process.out", NULL);
            } else {  // Parent
              currently = true;
              process.startTime = oldClk;
              process.PID = process_id;
              process.state = _RUNNING;
              printf("At time = %d, new process with ID = %d started running\n",
                     oldClk, process.id);
            }
          }
        } else {
          /* Print statement */
          printf("At time = %d, ID = %d, remaining time = %d\n", oldClk,
                 process.id, process.remainingTime);
        }
      }
      /************************************************************************/
    }
    /**************************************************************************/
  }
}

/**
 * @brief Shortest Remaining Time Next (SRTN) scheduling algorithm.
 *
 * This function implements the SRTN scheduling algorithm using processes
 * received from a message queue. It manages process execution based on their
 * remaining time to complete, using process suspension and resumption to ensure
 * the shortest job is always running.
 *
 * The function manages process forking, signaling (SIGSTOP, SIGCONT, SIGKILL),
 * and updates the state of each process in a priority queue.
 *
 * @details
 * - Initializes necessary variables and structures.
 * - Continuously receives processes until all processes are received and
 *   the priority queue is empty.
 * - Iterates in time steps (each second) and handles process forking,
 *   execution, and termination.
 *
 * @note
 * - Uses functions from Prio_Queue module for queue operations.
 * - Uses getClk() to get current clock time.
 * - Uses kill() to terminate processes.
 *
 * @warning
 * - Uses SIGKILL to forcefully terminate processes.
 *
 * @param None
 * @return None
 */
void SRTN(void) {
  printf("============ SRTN ============\n");
  /****************************** Initialization ******************************/
  int oldClk = getClk();  /**< Clock counter */
  struct PCB rec;         /**< PCB to receive processes */
  struct Prio_Queue q;    /**< Priority queue to implement the algorithm */
  struct PCB process;     /**< Process to be currently executed */
  bool currently = false; /**< Currently running a process */
  Prio_Queue_Init(&q);    /**< Initialize the priority queue */
  process.id = -1;        /**< To disable unreasonable stopping */
  /****************************************************************************/

  while ((receivedProcesses < processNumber) || !Prio_Queue_isEmpty(&q) ||
         (currently == true)) {
    /***************************** Receive Process ****************************/
    rec = rec_msg_queue();
    if (rec.id != -1) {
      Prio_Queue_enqueue(&q, rec.remainingTime, rec);
      /* Print Statement */
      printf("At time = %d, received process with ID = %d\n", getClk(), rec.id);
      if (currently == false) {
        currently = true;
        int process_id = fork();
        if (process_id == -1) {
          perror("Error in forking of a process ");
          exit(-1);
        } else if (process_id == 0) {  // Child
          execl("./process.out", "process.out", NULL);
        } else {  // Parent
          process = Prio_Queue_dequeue(&q);
          process.startTime = oldClk;
          process.PID = process_id;
          process.state = _RUNNING;
          printf("At time = %d, new process with ID = %d started running\n",
                 oldClk, process.id);
        }
      }
    }
    /**************************************************************************/

    /******************************** TIME STEP *******************************/
    if (getClk() - oldClk == 1) {
      Prio_Queue_Inc_WaitingTime(&q, oldClk);
      oldClk = getClk();
      /***************************** PAUSE RUNNING ****************************/
      if (process.id != -1 && process.state == _RUNNING) {
        currently = false;
        /* Pause it from running */
        kill(process.PID, SIGSTOP);
        /* Decrement the remaining time */
        process.remainingTime--;
        if (process.remainingTime == 0) {
          /* Kill the process */
          kill(process.PID, SIGKILL);
          /* Print statement */
          printf("At time = %d, process with ID = %d, has finished\n", oldClk,
                 process.id);
        } else {
          /* Set its state to ready to be run */
          process.state = _READY;
          /* Insert it back into the queue */
          Prio_Queue_enqueue(&q, process.remainingTime, process);
          /* Print statement */
          printf("At time = %d, ID = %d, remaining time = %d\n", oldClk,
                 process.id, process.remainingTime);
        }
      }
      /************************************************************************/

      /*************************** NORMAL PROCESSING **************************/
      process = Prio_Queue_dequeue(&q);
      if (process.id != -1) {
        currently = true;
        if (process.state == _READY) {
          process.state = _RUNNING;
          kill(process.PID, SIGCONT);
        } else if (process.state == _NEW) {
          int process_id = fork();
          if (process_id == -1) {
            perror("Error in forking of a process ");
            exit(-1);
          } else if (process_id == 0) {  // Child
            execl("./process.out", "process.out", NULL);
          } else {  // Parent
            process.startTime = oldClk;
            process.PID = process_id;
            process.state = _RUNNING;
            printf("At time = %d, new process with ID = %d started running\n",
                   oldClk, process.id);
          }
        }
      }
      /************************************************************************/
    }
    /**************************************************************************/
  }
}

/**
 * @brief Round-Robin (RR) scheduling algorithm.
 *
 * This function implements the Round-Robin scheduling algorithm for process
 * management and execution. It handles processes received from a message queue,
 * manages their execution using forked processes, and ensures fair allocation
 * of CPU time using a fixed time quantum.
 *
 * The function manages process forking, signaling (SIGSTOP, SIGCONT, SIGKILL),
 * and updates the state of each process in a circular queue.
 *
 * @details
 * - Initializes necessary variables and structures for managing the algorithm.
 * - Continuously receives processes until all processes are received and the
 *   circular queue is empty, while ensuring currently running processes are
 *   managed according to the Round-Robin scheduling policy.
 * - Iterates in time steps (quantumSize) and handles process forking,
 *   execution, pausing, and resuming.
 *
 * @note
 * - Uses functions like getClk() to get current clock time.
 * - Uses fork(), execl() for process creation and execution.
 * - Uses kill() to manage process states (SIGSTOP, SIGCONT, SIGKILL).
 *
 * @warning
 * - Uses SIGKILL to forcefully terminate processes.
 *
 * @param None
 * @return None
 */
void RR(void) {
  printf("============= RR =============\n");
  /****************************** Initialization ******************************/
  int oldClk = getClk();  /**< Clock counter */
  struct PCB rec;         /**< PCB to receive processes */
  struct Circ_Queue q;    /**< Circular queue to implement the algorithm */
  struct PCB process;     /**< Process to be currently executed */
  bool currently = false; /**< Currently running a process */
  Circ_Queue_Init(&q);    /**< Initialize the priority queue */
  process.id = -1;        /**< To disable unreasonable stopping */
  /****************************************************************************/

  while ((receivedProcesses < processNumber) || !Circ_Queue_isEmpty(&q) ||
         (currently == true)) {
    /***************************** Receive Process ****************************/
    rec = rec_msg_queue();
    if (rec.id != -1) {
      Circ_Queue_enqueue(&q, rec);
      /* Print Statement */
      printf("At time = %d, received process with ID = %d\n", getClk(), rec.id);
      if (currently == false) {
        currently = true;
        int process_id = fork();
        if (process_id == -1) {
          perror("Error in forking of a process ");
          exit(-1);
        } else if (process_id == 0) {  // Child
          execl("./process.out", "process.out", NULL);
        } else {  // Parent
          process = Circ_Queue_dequeue(&q);
          process.startTime = oldClk;
          process.PID = process_id;
          process.state = _RUNNING;
          printf("At time = %d, new process with ID = %d started running\n",
                 oldClk, process.id);
        }
      }
    }
    /**************************************************************************/

    /******************************** TIME STEP *******************************/
    if (getClk() - oldClk == quantumSize) {
      Circ_Queue_Inc_WaitingTime(&q, oldClk);
      oldClk = getClk();

      /***************************** PAUSE RUNNING ****************************/
      if (process.id != -1 && process.state == _RUNNING) {
        currently = false;
        /* Pause it from running */
        kill(process.PID, SIGSTOP);
        /* Decrement the remaining time */
        process.remainingTime -= quantumSize;
        if (process.remainingTime <= 0) {
          /* Kill the process */
          kill(process.PID, SIGKILL);
          /* Print statement */
          printf("At time = %d, process with ID = %d, has finished\n", oldClk,
                 process.id);
        } else {
          /* Set its state to ready to be run */
          process.state = _READY;
          /* Insert it back into the queue */
          Circ_Queue_enqueue(&q, process);
          /* Print statement */
          printf("At time = %d, ID = %d, remaining time = %d\n", oldClk,
                 process.id, process.remainingTime);
        }
      }
      /************************************************************************/

      /*************************** NORMAL PROCESSING **************************/
      process = Circ_Queue_dequeue(&q);
      if (process.id != -1) {
        currently = true;
        if (process.state == _READY) {
          process.state = _RUNNING;
          kill(process.PID, SIGCONT);
        } else if (process.state == _NEW) {
          int process_id = fork();
          if (process_id == -1) {
            perror("Error in forking of a process ");
            exit(-1);
          } else if (process_id == 0) {  // Child
            execl("./process.out", "process.out", NULL);
          } else {  // Parent
            process.startTime = oldClk;
            process.PID = process_id;
            process.state = _RUNNING;
            printf("At time = %d, new process with ID = %d started running\n",
                   oldClk, process.id);
          }
        }
      }
      /************************************************************************/
    }
    /**************************************************************************/
  }
}