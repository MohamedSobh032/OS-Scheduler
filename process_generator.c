#include <stdio.h>

#include "headers.h"

/******************** MACROS ********************/
#define __PROCESSES_FILE__ "processes.txt"
/************************************************/

/*************** Global Variables ***************/
int processesNum;      // NOLINT
PCB* pcbArray;         // NOLINT
Scheduling_Algo algo;  // NOLINT
int quantumSize;       // NOLINT
/************************************************/

/************* Function Definitions *************/
void clearResources(int);
int countLines(FILE* file);
void readFile(void);
void getAlgorithm(void);
/************************************************/

int main(int argc, char* argv[]) {
  signal(SIGINT, clearResources);
  // TODO Initialization
  // 1. Read the input files.
  readFile();
  // 2. Ask the user for the chosen scheduling algorithm and its parameters
  getAlgorithm();

  // 3. Initiate and create the scheduler and clock processes.
  // 4. Use this function after creating the clock process to initialize clock
  // initClk();
  // To get time use this
  // int x = getClk();
  // printf("current time is %d\n", x);
  // TODO Generation Main Loop
  // 5. Create a data structure for processes and provide it with its
  // parameters.
  // 6. Send the information to the scheduler at the appropriate time.
  // 7. Clear clock resources
  // destroyClk(true);
}

int countLines(FILE* file) {
  int count = 0;
  char buffer[50];
  while (fgets(buffer, sizeof(buffer), file)) {
    count++;
  }
  return count;
}

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

    index++;
  }
}

void getAlgorithm(void) {
  int userInput = 0;
  printf("Enter the scheduling algorithm (1-HPF, 2-SRTN, 3-RR): ");
  scanf("%d", &userInput);  // NOLINT
  switch (userInput) {
    case 0:
      algo = HPF;
      break;
    case 1:
      algo = SRTN;
      break;
    case 2:
      algo = RR;
      printf("Enter the quantum size: ");
      scanf("%d", &quantumSize);  // NOLINT
      break;
    default:
      perror("Wrong input");
      exit(-1);
      break;
  }
}

void clearResources(int signum) {
  // TODO Clears all resources in case of interruption
}
