#include <stdio.h>
#include <stdlib.h>

#include "bank.h"

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
                fprintf(stderr, "Error allocating available resource vector of size %d.\n", *numTypes);
                fclose(file);
                exit(1);
        }
        
        for(i = 0; i < *numTypes; i++) {
                fscanf(file, "%u", *available + i);
        }
        fclose(file);
        
        printf("Number of resource types = %d\n", *numTypes);
}


void display_msg(msgbuf_t * msgbuf, unsigned int numTypes) {
        int i;
        
        printf("\tClient id: %d\n", msgbuf->request.sender);
        if(msgbuf->request.inReply > 0)
        	printf("\tSerial number: %u\n",msgbuf->request.inReply);
	else
		printf("\tSerial number: %u\n",msgbuf->request.serialNum);
        
        printf("\tReturn address: queueID %u\n", msgbuf->request.retAddr);
        
        printf("\tMessage type: %d-", (int) msgbuf->mtype);
        switch(msgbuf->mtype) {
                case 1:
                        printf("Request");
                        break;
                case 2:
                        printf("Release");
                        break;
                case 3:
                        printf("Register");
                        break;
                case 11:
                        printf("Release All");
                        break;
                
                case 4:
                        printf("Request granted");
                        break;
                case 5:
                        printf("Request denied: unsafe");
                        break;
                case 6:
                        printf("Request denied: excessive");
                        break;
                case 12:
                        printf("Request denied: Unavailable");
                        break;
                        
                case 7:
                        printf("Release successful");
                        break;
                case 8:
                        printf("Release failed");
                        break;
                        
                case 9:
                        printf("Registration accepted");
                        break;
                case 10:
                        printf("Registration denied");
                        break;
                        
                default:
                        printf("UNKNOWN\n");
        }
        printf("\n");
        
        printf("\tresources: { ");
        for(i=0; i < numTypes; i++) {
                printf("%u, ", msgbuf->request.resourceVector[i]);
        }
        printf("}\n");
}
