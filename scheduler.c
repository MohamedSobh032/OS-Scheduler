#include "headers.h"

static int msg_id;       // NOLINT
static ssize_t rec_val;  // NOLINT

int main(int argc, char* argv[]) {
  initClk();

  key_t key_id = ftok(__FILE_KEY_NAME__, __FILE_KEY_VAL__);
  msg_id = msgget(key_id, 0666 | IPC_CREAT);
  if (msg_id == -1) {
    perror("Error in create");
    exit(-1);
  }
  struct msgbuff message;
  int i = 0;

  while (i < 4) {
    rec_val = msgrcv(msg_id, &message, sizeof(message.process), __MSG_TYPE__,
                     !IPC_NOWAIT);

    if (rec_val == -1) {
      perror("Error in receive");
    } else {
      printf("Process Received at time = %d\n", getClk());
      printf("ID = %d, AT = %d, RT = %d, Prio = %d\n", message.process.id,
             message.process.arrivalTime, message.process.runTime,
             message.process.prio);
      i++;
    }
  }

  // upon termination release the clock resources.
  destroyClk(true);
}
