#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define null 0

struct processData {
  int arrivaltime;
  int priority;
  int runningtime;
  int id;
  int memory;
};

int main(int argc, char* argv[]) {
  FILE* pFile = NULL;
  pFile = fopen("processes.txt", "w");
  int no = 0;
  struct processData pData;
  printf("Please enter the number of processes you want to generate: ");
  scanf("%d", &no);  // NOLINT
  srand(time(null));
  fprintf(pFile, "#id arrival runtime priority memory\n");
  pData.arrivaltime = 1;
  for (int i = 1; i <= no; i++) {
    // generate Data Randomly
    pData.id = i;
    pData.arrivaltime += rand() % (11);  // processes arrives in order
    pData.runningtime = rand() % (30);
    pData.priority = rand() % (11);
    pData.memory = rand() % (256);
    fprintf(pFile, "%d\t%d\t%d\t%d\t%d\n", pData.id, pData.arrivaltime,
            pData.runningtime, pData.priority, pData.memory);
  }
  fclose(pFile);
}
