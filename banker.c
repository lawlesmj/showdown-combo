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

typedef struct {
	unsigned int size;
	unsigned int items;
	client_info_t * clientInfo;
} sorted_array_t;

int compare_client_ids(const void* id, const void* pelem);

void add_client(sorted_array_t *, client_info_t);
client_info_t* get_client(sorted_array_t *, int id);
void remove_client(sorted_array_t *, int id);

void cleanup(int step, unsigned int * available, sorted_array_t clients, msgbuf_t * msgbuf,  int msgqid);


int main() {
	msgbuf_t * msgbuf;
	int msgqid;
	int clientqid;
	unsigned int numTypes;
	key_t key;
	int messageNum;
	
	unsigned int * available;
	sorted_array_t clients;
	
	char decreasingFlag;
	
	init_resources(&numTypes, &available);
	
	clients.size = 4;
	clients.items = 0;
	clients.clientInfo = (clients_info_t *) malloc(clients.size * sizeof(client_info_t))
	if(clients.clientInfo == NULL) {
		perror("malloc clients.clientInfo");
		cleanup(0, available, clients, msgbuf, msgqid);
		exit(1);
	}
	
	msgbuf = (msgbuf_t *) malloc(MSGBUF_SIZE);
	if(msgbuf == NULL) {
		perror("malloc msgbuf");
		cleanup(1, available, clients, msgbuf, msgqid);
		exit(1);
	}
	
	//Make exclusive mailbox
	key = BANK_MSGQUE_KEY;
	msgqid = msgget(key, 0420 | IPC_EXCL | IPC_CREAT);
	if(msgqui == NULL) {
		// Failed to create queue
		perror("msgget");
		cleanup(2, available, clients, msgbuf, msgqid);
		exit(1);
	}
	
	decreasingFlag = 0;
	messageNum = 0;
	do {
		//Wait for requests of any kind
		if( msgrcv(msgqui, msgbuf, REQUEST_SIZE, 0, 0) == -1) {
			//if there was an error:
			perror("msgrcv");
			cleanup(3, available, clients, msgbuf, msgqid);
			exit(1);
		};
		clientqid = msgbuf->request.sender;
		
		//display request
		printf("Received message at queueID %u:\n", msgqid);
		display_msg(msgbuf, numTypes);
		
		//process requests
		switch(msgbuf->mtype) {
			case 1:
				//client requesting resources
				break;
			case 2:
				//client releasing resources
				break;
			case 3:
				//client registering max resources
				
				break;
			case 11:
				//client releasing all resources
				break;
			default:
				//bad or unprocessed type, so ignore it
		}
		
		//display reply
		printf("Sent message to queueID %u:\n", clientqid);
		display_msg(msgbuf, numTypes);
		
		//Send reply
		
		//Display status
		
	} while(decreasingFlag && clients->items > 0);
	
	cleanup(-1, available, clients, msgbuf, msgqid);
	return 0;
}

//Put new guy sorted into the list
void add_client(sorted_array_t * clients, client_info_t* newClient) {
	unsigned int i;
	
	clients->items++;
	// Resize if needed
	if(clients->items > clients->size) {
		if(clients->size == 0) clients->size = 4;
		else				  clients->size *= 2;
		
		clients->clientInfo = realloc(clients->clientInfo, clients->size * sizeof(client_info_t));
		if(clients->clientInfo == NULL)
			perror("realloc clients->clientInfo");
	}
	// shift through array until position or end is found
	for(i = clients->items; i!=0 && clients->clientInfo[i-1]->id < newClient->id; i--) {
		clients->clientInfo[i] = clients->clientInfo[i-1];
	}
	clients->clientInfo[i] = newClient;
	
}

//Binary search for the given client
client_info_t* get_client(sorted_array_t * clients, int id) {
	return (client_info_t *) bsearch(&id, clients->clientInfo, 
		clients->items, sizeof(client_info_t *),
		compare_client_ids);
}

void remove_client(sorted_array_t * clients, int id) {
	client_info_t * removedClient;
	unsigned int low, high, mid;
	
	
	//find the client
	low = 0;
	high = clients->items;
	removedClient = NULL;
	while(low <= high) {
		mid = low + (high-low)/2;
		if(id < clients->clientInfo[mid]->id)
			high = mid - 1;
		else if(id > clients->clientInfo[mid]->id)
			low = mid + 1;
		else
			removedClient = clients->clientInfo[mid];
			break;
	}
	//if found
	if(removedClient != NULL) {
		//shift items to the left to fill the gap
		clients->items--;
		for(; mid < clients->items; mid++) {
			clients->clientInfo[mid] = clients->clientInfo[mid + 1];
		}
		
		//clean client info struct
		free(removedClient->allocated);
		free(removedClient->claims);
		free(removedClient->needs);
		//free client info struct
		free(removedClient);
	}
}

int compare_client_ids(const void* id, const void* pelem) {
	return *(int *)id - ((client_info_t *)pelem)->id;
}

void cleanup(int step, unsigned int * available, sorted_array_t clients, msgbuf_t * msgbuf,  int msgqid) {
	switch(step) {
		case -1:
			//all
		case 3:
			if(msgctl(msgqid, IPC_RMID, NULL) == -1)
				perror("msgctl");
		case 2:
			free(msgbuf);
		case 1:
			free(clients.clientInfo);
		case 0:
			free(available);
		default:
	}
}
