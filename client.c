#include <stdio.h>
#include <stdlib.h> //can use random because of this
#include <errno.h>
#include <string.h>

#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>

#include "banker.h"



int main(int argc, char *argv[]){
	msgbuf_t * request;
    int msgqid;
    unsigned int numTypes;
    key_t key;
    //manage message info
    int id;
    unsigned int * allocated;
    unsigned int * claim;
    
    init_resources(&numTypes, &claim);
    randomInit(&claim, numTypes);
    
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
    
}

void randomInit(int * claim, int size){
	int i = 0;
	for(i = 0; i < size; i++){
		claim[i] = (int) claim * rand();
	}
}
