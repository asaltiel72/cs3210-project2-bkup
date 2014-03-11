#include "gtallocator.h"
#include <stdlib.h>
#include <time.h>


#define USE_GTALLOC 1
#define USE_MALLOC 0

int type;
char command[7];
size_t alloc_size;
int num_of_allocs;


int main(int argc, char *argv[]) {
	if(argc != 4) {
		printf("memtest requires a type arguement, allocation size,"
	 	       "number of allocations.\nUse \"gtalloc\" or \"malloc\""
		       "for type.\nExample: memtest gtalloc 16 256");
		exit(1);
	}
	sscanf(argv[1], "%s", command);
	if(strcmp(command, "gtalloc") == 0) {
		type = USE_GTALLOC;	
	} else if(strcmp(command, "malloc") == 0) {
		type = USE_MALLOC;
	} else {
		printf("Invalid command\n");
		exit(1);
	}
	sscanf(argv[2], "%zu", &alloc_size);
	if(alloc_size < 1) {
		printf("Allocation size must be positive\n");
		printf("Value given: %zu", alloc_size); 
		exit(1);
	}	
	sscanf(argv[3], "%d", &num_of_allocs);
	if(num_of_allocs < 1) {
		printf("Number of allocations must be positive\n");
		printf("Value given: %d", num_of_allocs);
		exit(1);
	}
	printf("let's begin\n");	
	void *alloc_addrs[num_of_allocs];
	int index;
	time_t timer;
	struct tm* tm_info;
	if (type) {
		printf("gtalloc\n");
		//Display start time
		gt_init();
		printf("initializtion finished\n");
		time(&timer);
		tm_info = localtime(&timer);
		printf("Start time: %s\n", asctime(tm_info));
		
		//Start allocations
		for (index = 0; index < num_of_allocs; index++) {
			printf("alloc: %d\n", index);
			alloc_addrs[index] = gtalloc(alloc_size);
			printf("alloc addr = %p\n", alloc_addrs[index]);
		}
		
		//Display time allocations finish
		time(&timer);
		tm_info = localtime(&timer);
		printf("Allocs complete: %s\n", asctime(tm_info));
		
		//Start freeing
		for (index = 0; index < num_of_allocs; index++) {
			gtfree(alloc_addrs[index]);
		}

		//Display end time
		time(&timer);
		tm_info = localtime(&timer);
		printf("End time: %s\n", asctime(tm_info));
	} else {
		printf("malloc\n");
		//Display start time
		time(&timer);
		tm_info = localtime(&timer);
		printf("Start time: %s\n", asctime(tm_info));
		
		//Start allocations
		for (index = 0; index < num_of_allocs; index++) {
			printf("alloc: %d\n", index);
			alloc_addrs[index] = malloc(alloc_size);
		}
		
		//Display time allocations finish
		time(&timer);
		tm_info = localtime(&timer);
		printf("Allocs complete: %s\n", asctime(tm_info));
		
		//Start freeing
		for (index = 0; index < num_of_allocs; index++) {
			free(alloc_addrs[index]);
		}
		
		//Display end time
		time(&timer);
		tm_info = localtime(&timer);
		printf("End time: %s\n", asctime(tm_info));
	}
}
