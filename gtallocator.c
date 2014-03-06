#include "gtallocator.h"

size_t calc_prg_mem_size(int min_block, int total_block);
size_t get_requested_order(size_t bytes);
int find_free(size_t order);

/*
	TODO: 
		- DO EVERYTHING IMPORTANT ATOMICALLY
		- Initialize prg_mem to mmapped block for tree structure 
		- Initialize user_mem to mmapped block of INITIAL_BLOCK
		- Create functions to resize program and user space
*/

__attribute__ ((constructor)) void init() {
	
	user_mem = mmap(NULL, INITIAL_BLOCK, PROT_READ | PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
	if(user_mem == MAP_FAILED){
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
	if(prg_mem == MAP_FAILED){
		//print error and errno, then die?
	}
	num_sizes = (log2(INITIAL_BLOCK) - log2(MINIMUM_BLOCK));
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
	block *curr_block = first_map->free_list;
	int offset = 1;

	//initalize first block
	curr_list->size = block_size;
	curr_list->location_array = curr_block;
	curr_list->array_size = offset;
	curr_list->location_array->location = (uint32_t) usr_mem;
	curr_list->location_array->free = FREE;
	curr_list->location_array->buddy = NULL;
	
	//initialize the rest
	int i = 0;
	for(i = 1; i < num_sizes; i++) {
		block_size = block_size/2;
		curr_list[i].size = block_size;
		curr_list[i].location_array = curr_list[i-1].location_array + offset;
		offset = offset * 2;
		curr_list[i].array_size = offset;
	}
}

__attribute__ ((destructor)) void destory() {
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
    return NULL;
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

int find_free(size_t order){
	int i = 0;
	block *curr = curr_list[order].location_array;
	for(i = 0; i < curr_list[order].array_size; i++){
		if(curr[i].free){
			return i;
		}
	}
	return -1;
}

size_t calc_prg_mem_size(int min_block, int total_block) {
	int total_size;	
	int num_sizes = (log2(total_block) - log2(min_block));
	total_size = sizeof(map);
	total_size = total_size + (sizeof(block_list) * num_sizes);
        total_size = total_size + (sizeof(block) * FREE_ARRAY((num_sizes + 1), -1));
	return (size_t) total_size;
}

size_t get_requested_order(size_t bytes){
	return ((size_t) (ceil(log2((double) bytes))));
}

void split(size_t index) {
	int i = 1;
	int ret;
	do {
		ret = find_free(curr_list[index - i].location_array);
		i++;
	} while (ret == -1);
	block *curr_block;
	uint32_t temp_loc = curr_block->location;
	do {
		curr_block = &(curr_list[index - i].location_array[ret]);
		curr_block-> = taken;
		i--;
		ret = ret * 2;
		curr_block = &(curr_list[index - i].location_array[ret]);
		curr_block->free = TAKEN;
		curr_block->location = temp_loc;
		curr_block->buddy = &(curr_block[1]);
		curr_block = &(curr_block[1]);
		curr_block->free = FREE;
		curr_block->location = temp_loc	+ curr_list[index - i].size;
	       	curr_block->buddy = &(curr_block[-1]);
	} while (i > 1);

}	
