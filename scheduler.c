#include "headers.h"

/******************** MACROS ********************/
#define __PROCESS_NUMBER_ID__ 1
#define __ALGORITHM_NUMBER_ID__ 2
#define __QUANTUM_SIZE_ID__ 3
/************************************************/

/*************** Global Variables ***************/
static int msg_id;              // NOLINT
static ssize_t rec_val;         // NOLINT
static int processNumber;       // NOLINT
static int algo;                // NOLINT
static int quantumSize;         // NOLINT
static struct msgbuff message;  // NOLINT
/************************************************/

/************* Function Definitions *************/
struct PCB rec_msg_queue(void);
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
    pcb.startTime = message.process.startTime;
    pcb.prio = message.process.prio;
    pcb.state = _NEW;
    pcb.waitTime = 0;
  }
  return pcb;
}