#include <stdio.h>
#include <stdlib.h> //can use random because of this
#include <errno.h>
#include <string.h>

#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>

#include <unistd.h> //lets us use u sleep

#include "banker.h"

void randomInit(unsigned int request[]);
void randomRelease(unsigned int request[]);
void wait();
void cleanUp();

//globals
unsigned int * allocated;
unsigned int * claim;
unsigned int numTypes;
const int MAX_ATTEMPTS = 10;
const int MAX_ITERATIONS = 10;

int main(int argc, char *argv[]){
	msgbuf_t * msgbuf;
	msgbuf_t * respbuf;
	int myqid;
	int bankqid;
	
	key_t key;
	//manage message info
	int id;
	
	//manage loops
	unsigned int i,j;
	unsigned int attempts = 0;
    
    
	init_resources(&numTypes, &claim);
	allocated = (unsigned int *) calloc(numTypes,sizeof(unsigned int));
	randomInit(&claim, &allocated, numTypes);
   
    
	request = (msgbuf_t *) malloc(MSGBUF_SIZE);
    
	if(request == NULL) {
		printf("Error allocating message buffer of size %d.", numTypes);
		cleanUp(0, msgbuf, respbuf);
		exit(1);
    	}
    
	key = ftok("initial.data" , id);
    
	if((bankqid = msgget(key, 0644)) == -1) {
		perror("msgget");
		cleanUp(0, msgbuf, respbuf);
		exit(1);
  	}
	if(myqid = msgget(key,0644| IPC_CREAT | IPC_EXCL) == -1){
		printf("Cannot create an exclusive message queue exiting"):
		cleanUp(0, msgbuf, respbuf);
		exit(1);
	}
    
	msgbuf.mtype = 1;
	msgbuf->request.sender = id;
	msgbuf->request.serialNum = 1;
	msgbuf->request.retAddr = myqid;
	do{
		//setup message
		msgbuf->request.serialNum = msgbuf->request.serialNum += 1;
		memcpy( msgbuf->resourceVector, claim, sizeof(unsigned int) * numTypes);
		
		//send intial request
		if (msgsnd(bankqid, msgbuf, REQUEST_SIZE) == -1){
			printf("Error sending message");
			cleanUp(1, msgbuf, respbuf);
			exit(1);
		}
		if (msgrc(myqid, respbuf, REQUEST_SIZE, 0, 0) == -1){
			printf("Error recieving message");
			cleanUp(1, msgbuf, respbuf);
			exit(1);
		}
		for(i = 0; i < numTypes; i++){
			if(claim[i] > 0)
				claim[i] = claim[i] - 1;
		}
	}while(respbuf.mtype == 10);
	
    
	for(j = 0; j < MAX_ITERATIONS; j++){
		//step1 make a randomized resource request
		msgbuf->request.serialNum = msgbuf->request.serialNum += 1;
		randomInit(msgbuf->request.resourceVector);
		if (msgsnd(bankqid, msgbuf, REQUEST_SIZE) == -1){
			printf("Error sending message");
			exit(1);
		}
		if (msgrc(myqid, respbuf, REQUEST_SIZE, 0, 0) == -1){
			printf("Error recieving message");
			exit(1);
		}
	
		//step2 wait for a response from the banker of a matching serial number
		if(msgrcv(msgqid, note, NOTE_SIZE(max_string_length), 0, 0) == -1) {
			perror("msgrcv");
			exit(1);
		}
	
		//step3
		do{
			//send request
			switch(/*purpose code */){
				case 4:
					//request granted
					attempts = MAX_ATTEMPTS;
					break;
				case 6:
					// request denied exceeds max claims
					for(i = 0; i < numTypes; i++){
						msgbuf->request.data[i] = msgbuf->request.data[i] - 1;
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
					break;
			}
		}while(attempts < MAX_ATTEMPTS);
	
	
		if(attempts < MAX_ATTEMPTS){
			//step 4
			wait();
			//step 5
			randomRelease(/* put the stuff here */);
			//step 6
			//wait for response
			//step 7 
		
			if( purpose code == 8){
				//print some stuff
				//alter allocated vector accordingly
			}
			else{
				//print some stuff
				//do not alter allocated vector
			}
		
	
		}
	
		//step 8 
		wait();
	
		//reseting do-while loop
		//happens at the end of the loop
		attempts = 0;
	}
    
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

void cleanUp(int case; int * msgBuf, int * rspBuf){
	switch(case){
		case 0:
			free(msgbuf);
			free(rspbufp);
			msgctl(msgqid, IPC_RMID, NULL);	
		case 1:
			free(allocated);
			free(claim);
			break;
		default:
			//something went wrong clean everything just in case
			free(allocated);
			free(claim);
			free(msgbuf);
			free(rspbufp);
			msgctl(msgqid, IPC_RMID, NULL);	
			break;
				
	}
	
}

