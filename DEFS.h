
/**
 * @file DEFS.h
 * @brief Definitions used in project.
 */

#ifndef _DEFS_H_
#define _DEFS_H_

/**************** Global Macros *****************/
#define __FILE_KEY_NAME__ "keyfile"
#define __FILE_KEY_VAL__ 65
#define __MSG_TYPE__ 10
/************************************************/

/**
 * @brief Enum defining different states of a process.
 */
typedef enum ProcessState {
  _UNKNOWN = 0,   /**< Unknown state */
  _NEW = 1,       /**< New Process state */
  _READY = 2,     /**< Ready state */
  _RUNNING = 3,   /**< Running state */
  _BLOCKED = 4,   /**< Blocked state */
  _TERMINATED = 5 /**< Terminated state */
} ProcessState;

/**
 * @brief Struct representing Process Control Block (PCB) for a process.
 */
typedef struct PCB {
  int id;            /**< Unique identifier of the PCB */
  int PID;           /**< Process ID */
  int arrivalTime;   /**< Time at which the process arrives */
  int startTime;     /**< Time at which the process starts execution */
  int runTime;       /**< Total runtime required by the process */
  int remainingTime; /**< Decrementer to get the remaining time */
  int waitTime;      /**< Total waited time from start to end of the run */
  int endTime;       /**< Time at which the process finishes execution */
  int prio;          /**< Priority of the process */
  int memory;        /**< Memory required to allocate */
  enum ProcessState state; /**< Current state of the process */
  void* memPointer;        /**< Pointer to memory allocation */
} PCB;

/**
 * @brief Struct representing a message buffer for IPC containing a PCB.
 */
typedef struct msgbuff {
  long mtype;  /**< Message type */
  PCB process; /**< Process Control Block */
} msgbuff;

#endif /* _DEFS_H_ */