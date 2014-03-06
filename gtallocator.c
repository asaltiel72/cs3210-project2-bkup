#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
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
	size_t prg_space;
	prg_space = calc_prg_mem_size(MINIMUM_BLOCK, INITIAL_BLOCK);
	

	//prg_mem layout is as follows
	//a map struct that points to the next map (for expanding)
	//an array of block_lists for the allowed block sizes
	//an array of blocks
	//may need to add an array for freeing

	prg_mem = mmap(NULL, prg_space, PROT_READ | PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
	if(prm_mem == MAP_FAILED){
		//print error and errno, then die?
	}
	num_sizes = (log2(total_block) - log2(min_block));
	first_map = (map *) prg_mem;
	first_map->head = prg_mem + sizeof(map);
	first_map->free_list = prg_mem + sizeof(map) + 
					(sizeof(block_list) * num_sizes);
	
	//&head = prg_mem; // set our structure address to the created private memory segment
	//head->state = 1;
	//head->space = INITIAL_BLOCK;
	//head->location_array->location = usr_mem;
	//head->location_array->next = NULL;
	// change if circular list wanted
	//head->previous = NULL;
	//head->next = NULL;
	//head->buddy= NULL;
	
	uint32_t block_size = INITIAL_BLOCK;
	curr_list = first_map->head;
	block *curr_block = map->free_list;
	int offset = 1;

	//initalize first block
	curr_list->size = block_size;
	curr_list->location_array = curr_block;
	curr_list->array_size = offset;
	
	//initialize the rest
	for(int i = 1; i < num_sizes; i++) {
		block_size = block_size/2;
		curr_list[i]->size = block_size;
		curr_list[i]->location_array = curr_list[i-1]->location_array
					     + offest;
		offset = offset * 2;
		curr_list[i]->array_size = offset;
	}
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
    
}

void gtfree(void *addr){
	/*
		TODO:
			- Mark unallocated block as free
			(recursively)
			- Check buddy partitions
			- Merge if buddy is currently free
	*/
}

void * find_free(block *array){
	int i = 0;
	while(i < block->array_size){
		if(block[i].free){
			return ((void *) block[i].location);
		}
		i++;
	}
	return NULL;
}

size_t calc_prg_mem_size(int min_block, int total_block) {
	int total_size;	
	int num_sizes = (log2(total_block) - log2(min_block));
	total_size = sizeof(map);
	total_size = total_size + (sizeof(node) * num_sizes);
        total_size = total_size + (sizeof(free_addr) * 
				  FREE_ARRAY((num_sizes + 1), -1));
	return (size_t) total_size;
}

size_t get_requested_order(size_t bytes){
	return ((size_t) (ceil(log2((double) t))));
}

void split();
