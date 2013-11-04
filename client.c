#include <stdio.h>
#include <stdlib.h> //can use random because of this
#include <errno.h>
#include <string.h>

#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>

#include <unistd.h> //lets us use u sleep

#include "bank.h"

void randomInit(unsigned int request[]);
void randomRelease(unsigned int request[]);
void wait();
void cleanUp(int instance, msgbuf_t * msgBuf, msgbuf_t * rspBuf);

//globals
unsigned int * allocated;
unsigned int * claim;
unsigned int numTypes;
const int MAX_ATTEMPTS = 10;
const int MAX_ITERATIONS = 10;
int myqid;
int bankqid;

int main(int argc, char *argv[]){
        msgbuf_t * msgbuf;
        msgbuf_t * respbuf;
        unsigned int serial;
        
        key_t key;
        //manage message info
        int id = atoi(argv[0]);
        
        //manage loops
        unsigned int i,j;
        unsigned int attempts = 0;
    
    
        init_resources(&numTypes, &claim);
        allocated = (unsigned int *) calloc(numTypes,sizeof(unsigned int));
        randomInit(claim);
   
    
        msgbuf = (msgbuf_t *) malloc(MSGBUF_SIZE);
    
        if(msgbuf == NULL) {
                printf("Error allocating message buffer of size %d.", numTypes);
                cleanUp(1, msgbuf, respbuf);
                exit(1);
            }
    
        key = ftok("initial.data" , id);
    
        if((bankqid = msgget(key, 0644)) == -1) {
                printf("Cannot get the banker's address");
                cleanUp(1, msgbuf, respbuf);
                exit(1);
          }
        if(myqid = msgget(key,0644| IPC_CREAT | IPC_EXCL) == -1){
                printf("Cannot create an exclusive message queue exiting");
                cleanUp(1, msgbuf, respbuf);
                exit(1);
        }
    
        serial = id * 1000;
        msgbuf->mtype = 3;
        msgbuf->request.sender = id;
        msgbuf->request.serialNum = serial;
        msgbuf->request.retAddr = myqid;
        do{
                //setup message
                serial++;
                msgbuf->request.serialNum = msgbuf->request.serialNum = serial;
                memcpy( msgbuf->request.resourceVector, claim, sizeof(unsigned int) * numTypes);
                
                //display sent message
                printf("Sent message to queueID %u:\n", bankqid);
                display_msg(msgbuf, numTypes);
                
                //send intial request
                if (msgsnd(bankqid, msgbuf, REQUEST_SIZE, 0) == -1){
                        printf("Error sending message");
                        cleanUp(0, msgbuf, respbuf);
                        exit(1);
                }
                if (msgrcv(myqid, respbuf, REQUEST_SIZE, 0, 0) == -1){
                        printf("Error recieving message");
                        cleanUp(0, msgbuf, respbuf);
                        exit(1);
                }
                
                //display received message
                printf("Received message at queueID %u:\n", myqid);
                display_msg(msgbuf, numTypes);
                
                for(i = 0; i < numTypes; i++){
                        if(claim[i] > 0)
                                claim[i] = claim[i] - 1;
                }
        }while(respbuf->mtype == 10 && msgbuf->request.serialNum <= MAX_ITERATIONS );
        
    
        for(j = 0; j <= MAX_ITERATIONS; j++){
                //step1 make a randomized resource request
                msgbuf->mtype = 1;
                randomInit(msgbuf->request.resourceVector);
        
                do{
                        
                        
                        serial++;
                        msgbuf->request.serialNum = serial;
                
                        if (msgsnd(bankqid, msgbuf, REQUEST_SIZE, 0) == -1){
                                printf("Error sending message");
                                cleanUp(0, msgbuf, respbuf);
                                exit(1);
                        }
                        
                        //display sent message
                        printf("Sent message to queueID %u:\n", bankqid);
                        display_msg(msgbuf, numTypes);
                        
                        if (msgrcv(myqid, respbuf, REQUEST_SIZE, 0, 0) == -1){
                                printf("Error recieving message");
                                cleanUp(0, msgbuf, respbuf);
                                exit(1);
                        }
                        
                        if(respbuf->request.serialNum != serial){
                                printf("Banker error their banker serial# %d client serial# \n", respbuf->request.serialNum, serial);
                                cleanUp(0, msgbuf, respbuf);
                                exit(1);
                        }
                        
                        //display received message
                        printf("Received message at queueID %u:\n", bankqid);
                        display_msg(msgbuf, numTypes);
                        
                        switch(respbuf->mtype){
                                case 4:
                                        //request granted
                                        attempts = MAX_ATTEMPTS;
                                        break;
                                case 6:
                                        // request denied exceeds max claims
                                        for(i = 0; i < numTypes; i++){
                                                msgbuf->request.resourceVector[i] = msgbuf->request.resourceVector[i] - 1;
                                        }
                                        attempts++;
                                        break;
                                case 5:
                                        //deny request due to safety
                                case 12:
                                        //request denied no resources available, but may be later
                                        wait();
                                        break;
                                default:
                                        //invalid purpose code
                                        attempts++;
                                        break;
                        }
                }while(attempts < MAX_ATTEMPTS);
        
        
                if(attempts < MAX_ATTEMPTS){
                        //step 4
                        for(i = 0; i < numTypes; i++){
                                allocated[i] = allocated[i] + msgbuf->request.resourceVector[i];
                        }
                        wait();
                        //step 5
                        msgbuf->mtype = 2;
                        randomRelease(msgbuf->request.resourceVector);
                        serial++;
                        msgbuf->request.serialNum = serial;
                        //step 6
                        if (msgsnd(bankqid, msgbuf, REQUEST_SIZE, 0) == -1){
                                printf("Error sending message");
                                cleanUp(0, msgbuf, respbuf);
                                exit(1);
                        }
                        if (msgrcv(myqid, respbuf, REQUEST_SIZE, 0, 0) == -1){
                                printf("Error recieving message");
                                cleanUp(0, msgbuf, respbuf);
                                exit(1);
                        }
                        
                        if(respbuf->request.inReply != serial){
                                printf("Banker error their banker serial# %d client serial# \n", respbuf->request.inReply, serial);
                                cleanUp(0, msgbuf, respbuf);
                                exit(1);
                        }
                        //step 7
                
                        if( respbuf->mtype == 8){
                                
                                //alter allocated vector accordingly
                                for(i = 0; i < numTypes; i++){
                                        allocated[i] = allocated[i] - msgbuf->request.resourceVector[i];        
                                }
                                display_msg(msgbuf, numTypes);
                        }
                        else{
                                display_msg(msgbuf, numTypes);
                                //do not alter allocated vector
                        }
                
        
                }
        
                //step 8
                wait();
        
                //reseting do-while loop
                //happens at the end of the loop
                attempts = 0;
        }
        
        //release all
        msgbuf->mtype = 11;
        if (msgsnd(bankqid, msgbuf, REQUEST_SIZE, 0) == -1){
                printf("Error sending message");
                cleanUp(0, msgbuf, respbuf);
                exit(1);
        }
        
        exit(0);
    
}

void randomInit(unsigned int request[]){
        int i;
        for(i = 0; i < numTypes; i++){
                request[i] = (unsigned int) (claim[i] - allocated[i] )* rand();
        }
}

void randomRelease(unsigned int request[]){
        int i;
        for(i = 0; i < numTypes; i++){
                request[i] = (unsigned int) (allocated[i] )* rand();
        }
}

void wait(){
        unsigned int time = 1000 + 9000 * rand();
        usleep(time);
}

void cleanUp(int instance, msgbuf_t * msgBuf, msgbuf_t * rspBuf){
        switch(instance){
                case 0:
                        free(msgBuf);
                        free(rspBuf);
                        msgctl(myqid, IPC_RMID, NULL);        
                case 1:
                        free(allocated);
                        free(claim);
                        break;
                default:
                        //something went wrong clean everything just in case
                        free(allocated);
                        free(claim);
                        free(msgBuf);
                        free(rspBuf);
                        msgctl(myqid, IPC_RMID, NULL);        
                        break;
                                
        }
        
}
   

