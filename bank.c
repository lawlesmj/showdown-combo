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
		printf("Error allocating available resource vector of size %d.", *numTypes);
		fclose(file);
		exit(1);
	}
	
	for(i = 0; i < *numTypes; i++) {
		fscanf(file, "%u", *available + i);
	}
	
	fclose(file);
}