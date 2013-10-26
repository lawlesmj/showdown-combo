#include <stdio.h>
#include <stdlib.h> //can use random because of this
#include <errno.h>
#include <string.h>

#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>

#include "banker.h"

void randomInit(unsigned int request[]);

//globals
unsigned int * allocated;
unsigned int * claim;
unsigned int numTypes;

int main(int argc, char *argv[]){
	msgbuf_t * msgbuf;
	int msgqid;
	
	key_t key;
    //manage message info
	int id;
    
    
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
    
    //
    randomInit(msgbuf->request.data[])
    
}

void randomInit(unsigned int request[]){
	int i;
	for(i = 0; i < numTypes; i++){
		request[i] = ((unsigned int) (claim[i] - allocated[i] )* rand();
	}
}

