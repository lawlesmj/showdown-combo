/**************************************************************************
* CS-450 Fall 2013
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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>

#include <tuple> //could be useful
//example instantiation: tuple <double, char> t2(3.55, 'a');
//example access: val1=get<0> (t2);//3.55
//we would need to keep it well order thats the biggest issue I see here.

#include "banker.h"


const unsigned int MAX_CLIENTS = 10;

void init_resources(unsigned int * numTypes, unsigned int ** available) {
	unsigned int i;
	FILE * file;
	//need to define an array that holds both the id of the client as well as their resources
	//this could either be done by assuming that the number the client says will be their id
	//or we could have a tuple with the first coord as the id and the second part as the resource matrix.
	// I'm not sure what would work better. Can you even do Tuples in C? Yes you can.
	
	file = fopen("initial.data", "r");
	if(file == NULL) {
		perror("fopen");
		exit(1);
	}
	
	fscanf(file, "%u", numTypes);
	
	*available = (unsigned int *) malloc(sizeof(unsigned int) * (*numTypes));
	
	if(*available == NULL) {
		printf("Error allocating available resource vector of size %d.", *numTypes);
		fclose(file);
		exit(1);
	}
	
	for(i = 0; i < *numTypes; i++) {
		fscanf(file, "%u", *available + i);
	}
	
	fclose(file);
}

int main() {
	msgbuf_t * msgbuf;
	int msgqid;
	unsigned int numTypes;
	key_t key;
	
	unsigned int * available;
	matrix_t clientClaim, clientAllocated, clientNeeds;
	
	usigned int numClients = 0;
	
	init_resources(&numTypes, &available);
	
	key = BANK_MSGQUE_KEY;
	
	return 0;
}
