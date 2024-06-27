typedef enum { HPF = 0, SRTN = 1, RR = 2 } Scheduling_Algo;
typedef enum { UNKNOWN = 0, READY = 1, RUNNING = 2, BLOCKED = 3 } STATE;

typedef struct {
  int id;
  int PID;
  int arrivalTime;
  int startTime;
  int runTime;
  int endTime;
  int prio;
  STATE state;
} PCB;

typedef struct msgbuff {
  long mtype;
  PCB process;
} msgbuff;