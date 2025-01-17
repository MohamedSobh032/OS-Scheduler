#include <errno.h>
#include <signal.h>
#include <stdio.h>  //if you don't use scanf/printf change this include
#include <stdlib.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
typedef short bool;
#define true 1
#define false 0

#include "DEFS.h"
#include "Data_Structures/CircQueue.h"
#include "Data_Structures/PrioQueue.h"
#include "Data_Structures/Queue.h"
#include "MemoryManager.h"

#define SHKEY 300

///==============================
int *shmaddr;  // NOLINT
//===============================

int getClk() { return *shmaddr; }

/*
 * All process call this function at the beginning to establish communication
 * between them and the clock module.
 */
void initClk() {
  int shmid = shmget(SHKEY, 4, 0444);
  while ((int)shmid == -1) {
    // Make sure that the clock exists
    printf("Wait! The clock not initialized yet!\n");
    sleep(1);
    shmid = shmget(SHKEY, 4, 0444);
  }
  shmaddr = (int *)shmat(shmid, (void *)0, 0);
}

/*
 * All process call this function at the end to release the communication
 * resources between them and the clock module.
 * Input: terminateAll: a flag to indicate whether that this is the end of
 * simulation. It terminates the whole system and releases resources.
 */

void destroyClk(bool terminateAll) {
  shmdt(shmaddr);
  if (terminateAll) {
    killpg(getpgrp(), SIGINT);
  }
}