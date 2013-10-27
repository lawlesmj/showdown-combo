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
	client_info_t * clientsInfo;
} sorted_array_t;

int compare_client_ids(const void* pid, const void* pelem);

void add_client(sorted_array_t *, client_info_t);
client_info_t* get_client(sorted_array_t *, int id);
void remove_client(sorted_array_t *, int id);
//void clear_clients(client_info_t[]);


int main() {
	msgbuf_t * msgbuf;
	int msgqid;
	unsigned int numTypes;
	key_t key;
	
	unsigned int * available;
	sorted_array_t clients;
	
	usigned int numClients = 0;
	
	init_resources(&numTypes, &available);
	
	key = BANK_MSGQUE_KEY;
	
	clients.size = 4;
	clients.items = 0;
	clients.clientsInfo = (clients_info_t *) malloc(clients.size * sizeof(client_info_t))
	
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
	
	if(removedClient != NULL) {
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

int compare_client_ids(const void* pid, const void* pelem) {
	return *(int *)pid - ((client_info_t *)pelem)->id;
}
