/**
 * @file process_generator.c
 * @brief Generates processes to send to the scheduler, takes the wanted
 * algorithm from the user as well as the quantum size
 */

#include "headers.h"

/******************** MACROS ********************/
#define __PROCESSES_FILE__ "processes.txt" /**< Default processes file */
/************************************************/

/*************** Global Variables ***************/
static int processesNum; /**< Number of processes */          // NOLINT
static PCB* pcbArray; /**< Array of PCBs */                   // NOLINT
static int algo; /**< Chosen scheduling algorithm */          // NOLINT
static int quantumSize; /**< Quantum size for Round Robin */  // NOLINT
static int msg_id; /**< Message queue ID */                   // NOLINT
/************************************************/

/************* Function Definitions *************/
void clearResources(int);
int countLines(FILE* file);
void readFile(void);
void getAlgorithm(void);
void forkClkandScheduler(void);
void sendProcesses(void);
/************************************************/

int main(int argc, char* argv[]) {
  signal(SIGINT, clearResources);
  // 1. Read the input files.
  readFile();
  // 2. Ask the user for the chosen scheduling algorithm and its parameters
  getAlgorithm();
  // 3. Initiate and create the scheduler and clock processes.
  forkClkandScheduler();
  // 4. Use this function after creating the clock process to initialize clock
  initClk();
  // 5. Send the information to the scheduler at the appropriate time.
  sendProcesses();
  // 6. Clear clock resources
  destroyClk(false);
}

/**
 * @brief Counts the number of lines in a given file.
 *
 * @param file Pointer to the file to be counted.
 * @return Number of lines in the file.
 */
int countLines(FILE* file) {
  int count = 0;
  char buffer[50];
  while (fgets(buffer, sizeof(buffer), file)) {
    count++;
  }
  return count;
}

/**
 * @brief Reads process information from a file and initializes PCBs.
 */
void readFile(void) {
  FILE* file = fopen(__PROCESSES_FILE__, "r");
  if (file == NULL) {
    perror("Error opening file");
    exit(-1);
  }
  processesNum = countLines(file) - 1;
  pcbArray = (PCB*)malloc(processesNum * sizeof(PCB));  // NOLINT
  if (pcbArray == NULL) {
    perror("Memory allocation failed");
    fclose(file);
    exit(-1);
  }
  char buffer[50];
  int index = 0;
  fseek(file, 0, SEEK_SET);
  fgets(buffer, sizeof(buffer), file);
  // Read and parse each PCB line
  while (fgets(buffer, sizeof(buffer), file)) {
    sscanf(buffer, "%d %d %d %d", &pcbArray[index].id,  // NOLINT
           &pcbArray[index].arrivalTime, &pcbArray[index].runTime,
           &pcbArray[index].prio);
    pcbArray[index].PID = 0;
    pcbArray[index].startTime = 0;
    pcbArray[index].endTime = 0;
    pcbArray[index].state = UNKNOWN;
    index++;
  }
}

/**
 * @brief Prompts user to select a scheduling algorithm and set its parameters.
 */
void getAlgorithm(void) {
  printf("[0]HPF   [1]SRTN   [2]RR\n");
  printf("Please, choose a scheduling algo: ");
  scanf("%d", &algo);  // NOLINT
  switch (algo) {
    case 0 || 1:
      break;
    case 2:
      printf("Enter the quantum size: ");
      scanf("%d", &quantumSize);  // NOLINT
      break;
    default:
      perror("Wrong input");
      exit(-1);
      break;
  }
}

/**
 * @brief Forks clock and scheduler processes.
 */
void forkClkandScheduler(void) {
  // Fork clock
  int clock_pid = fork();
  if (clock_pid == -1) {
    perror("Error in forking of clock");
    exit(-1);
  } else if (clock_pid == 0) {
    execl("./clk.out", "clk.out", NULL);
    perror("Error in clock");
    exit(-1);
  }
  // Fork scheduler
  int sch_pid = fork();
  if (sch_pid == -1) {
    perror("Error in forking of scheduler");
    exit(-1);
  } else if (sch_pid == 0) {
    // Convert parameters into char* and jump into scheduler
    char pnum[5], algonum[5], quantumnum[5];
    sprintf(pnum, "%d", processesNum);       // NOLINT
    sprintf(algonum, "%d", algo);            // NOLINT
    sprintf(quantumnum, "%d", quantumSize);  // NOLINT
    execl("./scheduler.out", "scheduler.out", pnum, algonum, quantumnum, NULL);
    perror("Error in scheduler");
    exit(-1);
  }
}

/**
 * @brief Sends processes to scheduler via message queue based on their arrival
 * time.
 */
void sendProcesses(void) {
  key_t key_id = ftok("keyfile", 1);
  msg_id = msgget(key_id, IPC_CREAT | 0666);
  if (msg_id == -1) {
    perror("Failed to create message queue");
    exit(-1);
  }
  msgbuff message;
  int i = 0;
  while (i < processesNum) {
    if (pcbArray[i].arrivalTime == getClk()) {
      message.process = pcbArray[i];
      int send_val =
          msgsnd(msg_id, &message, sizeof(message.process), !IPC_NOWAIT);
      if (send_val == -1) {
        perror("Failed to send in message queue");
        exit(-1);
      } else {
        i++;
      }
    }
  }
}

/**
 * @brief Cleans up resources like message queue upon receiving SIGINT signal.
 *
 * @param signum Signal number received.
 */
void clearResources(int signum) {
  msgctl(msg_id, IPC_RMID, NULL);
  printf("IPC instances are destroyed\n");
  exit(0);
}
