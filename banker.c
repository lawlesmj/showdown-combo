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

#include "bank.h"

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

void cleanup(int step, unsigned int * available, maxAvailable,
	sorted_array_t clients, msgbuf_t * msgbuf,  int msgqid);
	
int bankers_algorithm(sorted_array_t clients, unsigned int available[]);


int main() {
	msgbuf_t * msgbuf;
	int msgqid;
	int clientqid;
	unsigned int numTypes;
	key_t key;
	int messageNum;
	
	unsigned int * available;
	unsigned int * maxAvailable;
	sorted_array_t clients;
	client_info_t  * client;
	
	char decreasingFlag;
	
	//variables for the do while loop
	int i;
	int flag;
	
	init_resources(&numTypes, &available);
	
	maxAvailable = (unsigned int *) malloc(numTypes * sizeof(unsigned int));
	if(maxAvailable == NULL) {
		perror("malloc maxAvailable");
		cleanup(0, available, maxAvailable, clients, msgbuf, msgqid);
		exit(1);
	}
	memcpy(maxAvailable, available, numtypes * sizeof(unsigned int));
	
	clients.size = 4;
	clients.items = 0;
	clients.clientInfo = (clients_info_t *) malloc(clients.size * sizeof(client_info_t))
	if(clients.clientInfo == NULL) {
		perror("malloc clients.clientInfo");
		cleanup(1, available, maxAvailable, clients, msgbuf, msgqid);
		exit(1);
	}
	
	msgbuf = (msgbuf_t *) malloc(MSGBUF_SIZE);
	if(msgbuf == NULL) {
		perror("malloc msgbuf");
		cleanup(2, available, maxAvailable, clients, msgbuf, msgqid);
		exit(1);
	}
	
	//Make exclusive mailbox
	key = BANK_MSGQUE_KEY;
	msgqid = msgget(key, 0420 | IPC_EXCL | IPC_CREAT);
	if(msgqui == NULL) {
		// Failed to create queue
		perror("msgget");
		cleanup(3, available, maxAvailable, clients, msgbuf, msgqid);
		exit(1);
	}
	
	decreasingFlag = 0;
	messageNum = 1;
	do {
		//Wait for requests of any kind
		if( msgrcv(msgqui, msgbuf, REQUEST_SIZE, 0, 0) == -1) {
			//if there was an error:
			perror("msgrcv");
			cleanup(4, available, maxAvailable, clients, msgbuf, msgqid);
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
				client = get_client(clients, msgbuf->request.sender);
				for(i=0, flag=0; i<numTypes || flag; i++) {
					//calculate client needs
					client->needs[i] = client->claims[i] - client->allocated[i];
					//Does client request fit they're max claims?
					flag = msgbuf->request.resourceVector[i] > client->needs[i];
				}
				
				//No
				if(flag) {
					msgbuf->mtype = 6;
					printf("\tFAILED REQUEST\n");
				}
				else {
					//Yes
					for(i=0, flag=0; i<numTypes || flag; i++) {
						//do we have the resources available?
						flag = msgbuf->request.resourceVector[i] > available[i];
					}
					//No
					if(flag) {
						msbuf->mtype = 12;
						printf("\tFAILED REQUEST\n");
					}
					else {
						//Yes
						client = get_client(clients, msgbuf->request.sender);
						for(i=0; i<numTypes; i++) {
							client->allocated[i] += msgbuf->request.resourceVector[i];
							client->needs[i] -= msgbuf->request.resourceVector[i];
							available -= msgbuf->request.resourceVector[i];
						}
						if(bankers_algorithm(clients, available)) {
							//success
							msgbuf->mtype = 4;
							printf("\tGRANTED REQUEST\n");
						}
						else {
							//failure
							client->allocated[i] -= msgbuf->request.resourceVector[i];
							client->needs[i] += msgbuf->request.resourceVector[i];
							available += msgbuf->request.resourceVector[i];
							
							msgbuf->mtype = 5;
							printf("\tFAILED REQUEST\n");
						}
					}
				}
				break;
			case 2:
				//client releasing resources
				client = get_client(clients, msgbuf->request.sender);
				for(i=0, flag=0; i<numTypes || flag; i++) {
					//are they releasing resources that they actually have?
					flag = msgbuf->request.resourceVector[i] > client->allocated[i];
				}
				if(!flag) {
					//yes
					for(i=0; i<numTypes; i++) {
						available[i] += msgbuf->request.resourceVector[i];
						client->allocated -= msgbuf->request.resourceVector[i];
					}
					msgbuf->mtype = 7;
					
					
					printf("\tGRANTED RELEASE\n");
				}
				else {
					//No
					msgbuf->mtype = 8;
					
					printf("\tDENIED RELEASE\n");
				}
				break;
			case 3:
				//client registering max resources
				for(i=0, flag=0; i<numTypes || flag; i++) {
					//does the claim fit my max resources?
					flag = msgbuf->request.resourceVector[i] > maxAvailable[i]);
				}
				if(!flag) {
					//this claim will fit my max resources
					//I shall register you
					client = (client_info_t *) malloc(sizeof(client_info_t));
					if(client == NULL) {
						perror("malloc client info");
						flag = 1;
					}
					else {
						client->claims = (unsigned int *) malloc(sizeof(unsigned int) * numTypes);
						client->allocated = (unsigned int *) calloc(numtypes, sizeof(unsigned int));
						client->needs = (unsigned int *) malloc(sizeof(unsigned int) * numTypes);
						
						if(client->claims == NULL 
							|| client->allocated == NULL
							|| client->needs == NULL) {
							//failed to allocate arrays
							perror("malloc client resource arrays");
							free(client->claims);
							free(client->allocated);
							free(client->needs);
							free(client);
							flag = 1;
						}
						else {
							add_client(&clients, client);
							memcpy(client->claims, msgbuf->request.resourceVector,
								numTypes * sizeof(unsigned int));
							
							//send success message
							msgbuf->mtype = 9;
						}
					}
				}
				
				if(flag) {
					//this claim will not fit my max resources
					msgbuf->mtype = 10;
				}
				break;
			case 11:
				//client releasing all resources
				client = get_client(clients, msgbuf->request.sender);
				for(i=0; i<numTypes; i++) {
					available[i] += client->allocated[i];
				}
				remove_client(clients, msgbuf->request.sender);
				break;
			default:
				//bad or unprocessed type, so ignore it
		}
		
		msgbuf->request.sender = 0;
		msgbuf->request.inReply = messageNum;
		msgbuf->request.retAddr = msgqid;
		//display reply
		printf("Sent message to queueID %u:\n", clientqid);
		display_msg(msgbuf, numTypes);
		
		//Send reply
		msgsend(clientqid, msgbuf, REQUEST_SIZE, 0);
		messageNum++;
		
		//Display status
		
	} while(decreasingFlag && clients->items > 0);
	
	cleanup(-1, available, maxAvailable, clients, msgbuf, msgqid);
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
		if(clients->clientInfo == NULL) {
			perror("realloc clients->clientInfo");
			clients->size /= 2;
		}
	}
	
	if(clients->items <= clients->size) {
		// shift through array until position or end is found
		for(i = clients->items; i!=0 && clients->clientInfo[i-1]->id < newClient->id; i--) {
			clients->clientInfo[i] = clients->clientInfo[i-1];
		}
		clients->clientInfo[i] = newClient;
	}
	else
		client->items--;
	
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

void cleanup(int step, unsigned int * available, unsigned int * maxAvailable,
	sorted_array_t clients, msgbuf_t * msgbuf,  int msgqid) {
	switch(step) {
		case -1:
			//all
		case 4:
			if(msgctl(msgqid, IPC_RMID, NULL) == -1)
				perror("msgctl");
		case 3:
			free(msgbuf);
		case 2:
			free(clients.clientInfo);
		case 1:
			free(maxAvailable);
		case 0:
			free(available);
		default:
	}
}

int bankers_algorithm(sorted_array_t clients, unsigned int available[]) {
	return 1;
}
