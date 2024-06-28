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

void HPF(void) {
  /****************************** Initialization ******************************/
  int oldClk = getClk();  /* Clock counter */
  struct PCB rec;         /* PCB to receive processes */
  struct Prio_Queue q;    /* Priority queue to implement the algorithm */
  struct PCB process;     /* Process to be currently executed */
  bool currently = false; /* Currently running a process */
  Prio_Queue_Init(&q);    /* Initialize the priority queue */
  /****************************************************************************/

  while ((receivedProcesses <= processNumber) || !Prio_Queue_isEmpty(&q)) {
    /***************************** Receive Process ****************************/
    rec = rec_msg_queue();
    if (rec.id != -1) {
      Prio_Queue_enqueue(&q, rec.prio, rec);
    }
    /**************************************************************************/

    /******************************** TIME STEP *******************************/
    if ((getClk() - oldClk) == 1) {
      /* Clock handling to iterate each second */
      oldClk = getClk();

      /************************** NEW PROCESS FORKING *************************/
      if (currently == false) {
        /* Dequeue the head of the queue (highest priority process) */
        process = Prio_Queue_dequeue(&q);
        if (process.id != -1) {
          currently = true;
          /* Fork new process */
          int process_id = fork();
          if (process_id == -1) {
            perror("Error in forking of a process ");
            exit(-1);
          } else if (process_id == 0) {  // Child
            execl("./process.out", "process.out", NULL);
          } else {  // Parent
            process.startTime = oldClk;
            process.PID = process_id;
          }
        }
      }
      /************************************************************************/

      /************************** NORMAL PROCESSING  **************************/
      else {
        currently = false;
        process.remainingTime--;
        /* Destroy the running process */
        if (process.remainingTime == 0) {
          process.endTime = oldClk;
          kill(process.PID, SIGKILL);
        }
      }
      /************************************************************************/

      Prio_Queue_Inc_WaitingTime(&q, oldClk);
    }
    /**************************************************************************/
  }
}

void SRTN(void) {}

void RR(void) {}