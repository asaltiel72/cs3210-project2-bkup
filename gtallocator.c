#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h> //for brk & sbrk

/*
	TODO: 
		- DO EVERYTHING IMPORTANT ATOMICALLY
		- Initialize prg_mem to mmapped block for tree structure 
		- Initialize user_mem to mmapped block of INITIAL_BLOCK
		- Create functions to resize program and user space
*/

void init() __attribute__ ((constructor)) {
	
	usr_mem = mmap(NULL, INITIAL_BLOCK, PROT_READ | PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
	if(usr_mem == MAP_FAILED){
		//print error and errno, then die?
	}
	
	prg_mem = mmap(NULL, PRG_SPACE, PROT_READ | PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
	if(prm_mem == MAP_FAILED){
		//print error and errno, then die?
	}
	
	&head = prg_mem; // set our structure address to the created private memory segment
	head->is_available = 1;
	head->space = INITIAL_BLOCK;
	head->location_array->location = usr_mem;
	head->location_array->next = NULL;
	// change if circular list wanted
	head->previous = NULL;
	head->next = NULL;
}

void destory() __attribute__ ((destructor)) {
	// unmap all the mmapped regions
}


void * gtalloc(size_t bytes){
	/*
		TODO:
			- Implement best-fit algorithm to find and assign memory
			- Update memory tree
			- Log user allocation
			- Return usable pointer to user
	*/
    if (bytes == 0) {
		return NULL;
    }
    
    struct node *curr = head;
    int allocated = 0;
    while(!allocated && curr != NULL){
    	if(!curr->is_available || curr->size < bytes){
    		curr = curr->next;
    	} else {
    		/*
    			TODO: figure out how to find best-fit location
    				- iterate through free-location list?
    				- handle location splits
    		*/
    	}
    }
    
    if(!allocated){
    	
    }
}

void gtfree(void * addr){
	/*
		TODO:
			- Mark unallocated block as free
			(recursively)
			- Check buddy partitions
			- Merge if buddy is currently free
	*/
}


