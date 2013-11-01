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

#ifndef BANK_H
#define BANK_H

#define BANK_MSGQUE_KEY 0xF0540DA4

typedef struct {
  long mtype; //Message purpose code 
  struct {
    unsigned int sender; //defined by the person who instantiates the client
    unsigned int serialNum; //message number
    unsigned int 
    int retAddr; // return address for the reply
    //butts?
    int resourceVector[1];
  } request;
} msgbuf_t;

// Macro uses the numTypes varible to calculate the size of the message
#define REQUEST_SIZE (sizeof(msgbuf) - sizeof(long) + sizeof(int) * (numTypes - 1))
#define MSGBUF_SIZE (sizeof(msgbuf) + sizeof(int) * (numTypes - 1))

void init_resources(unsigned int * numTypes, unsigned int ** available);
void display_msg(msgbuf_t * msgbuf, unsigned int numTypes);


#endif
