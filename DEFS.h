
/**
 * @file DEFS.h
 * @brief Definitions used in project.
 */

#ifndef _DEFS_H_
#define _DEFS_H_

/**
 * @brief Enum defining different scheduling algorithms.
 */
typedef enum {
  HPF = 0,  /**< Highest Priority First */
  SRTN = 1, /**< Shortest Remaining Time Next */
  RR = 2    /**< Round Robin */
} algorithm;

/**
 * @brief Enum defining different states of a process.
 */
typedef enum {
  UNKNOWN = 0,   /**< Unknown state */
  READY = 1,     /**< Ready state */
  RUNNING = 2,   /**< Running state */
  BLOCKED = 3,   /**< Blocked state */
  TERMINATED = 4 /**< Terminated state */
} ProcessState;

/**
 * @brief Struct representing Process Control Block (PCB) for a process.
 */
typedef struct PCB {
  int id;             /**< Unique identifier of the PCB */
  int PID;            /**< Process ID */
  int arrivalTime;    /**< Time at which the process arrives */
  int startTime;      /**< Time at which the process starts execution */
  int runTime;        /**< Total runtime required by the process */
  int endTime;        /**< Time at which the process finishes execution */
  int prio;           /**< Priority of the process */
  ProcessState state; /**< Current state of the process */
} PCB;

/**
 * @brief Struct representing a message buffer for IPC containing a PCB.
 */
typedef struct msgbuff {
  long mtype;  /**< Message type */
  PCB process; /**< Process Control Block */
} msgbuff;

#endif /* _DEFS_H_ */