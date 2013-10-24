/**************************************************************************
*                 CS-450 Fall 2013
*
*Programming asignment #2: Deadlock Avoidance using the Banker's Algorithm
*
*Written By:
* -Douglas Applegate
* -Mike Lawless
*
*Submitted on: 11/03/2013
*
* **************************************************************************/

#ifndef BANKER_H
#define BANKER_H

#define BANK_MSGQUE_KEY 0xF7EA

typedef struct {
  long mtype; //Message purpose code 
  typedef struct {
    unsigned int sender; //defined by the person who instantiates the client
    unsigned int serialNum; //message number
    int retAddr; // return address for the reply
    //butts?
    int resourceVector[1];
  } request;
} msgbuf_t;

#define NOTE_SIZE sizeof(msgbuf) - sizeof(long)

#endif
