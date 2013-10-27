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

//globals
unsigned int * allocated;
unsigned int * claim;
unsigned int numTypes;
const int MAX_ATTEMPTS = 10;

int main(int argc, char *argv[]){
	msgbuf_t * msgbuf;
	int msgqid;
	
	key_t key;
	//manage message info
	int id;
	
	//manage loops
	unsigned int i;
	unsigned int attempts = 0;
    
    
	init_resources(&numTypes, &claim);
	allocated = (unsigned int *) calloc(numTypes,sizeof(unsigned int));
	randomInit(&claim, &allocated, numTypes);
   
    
    request = (msgbuf_t *) malloc(sizeof(msgbuf_t) + sizeof(int) * (numTypes - 1));
    
	if(request == NULL) {
		printf("Error allocating message buffer of size %d.", numTypes);
		exit(1);
    	}
    
	key = ftok("initial.data" , id);
    
	if(msgqid = msgget(key,0644| IPC_CREAT | IPC_EXCL) == -1){
		printf("Cannot create an exclusive message queue exiting"):
		exit(1);
	}
    
	//step1 make a randomized resource request
	randomInit(msgbuf->request.data);
	
	//step2 wait for a response from the banker of a matching serial number
	
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
		
	
	}
	
	//step 8 
	wait();
	
	//reseting do-while loop
	//happens at the end of the loop
	attempts = 0;
	
    
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
;}

