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

#include "banker.h"

typedef struct {
	int id;
	unsigned int * claims;
	unsigned int * allocated;
	unsigned int * needs;
	int done;
} client_info_t;

void init_resources(unsigned int * numTypes, unsigned int ** available) {
	unsigned int i;
	FILE * file;
	
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
	client_info_t * clientInfo;
	
	usigned int numClients = 0;
	
	init_resources(&numTypes, &available);
	
	key = BANK_MSGQUE_KEY;
	
	clientInfo = (client_info_t *) malloc(4 * sizeof(client_info_t))
	
	return 0;
}
