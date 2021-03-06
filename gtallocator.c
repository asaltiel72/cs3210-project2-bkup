#include "gtallocator.h"


size_t calc_prg_mem_size(int min_block, int total_block);
size_t get_requested_order(size_t bytes);
int find_free(size_t order);
void add_alloc(size_t order, int offset);
void remove_alloc(rl_node *node);
void * split(size_t order);
void merge(rl_node *free_node);
void mem_dump();

/*
	TODO: 
		- DO EVERYTHING IMPORTANT ATOMICALLY
		- Initialize prg_mem to mmapped block for tree structure 
		- Initialize user_mem to mmapped block of INITIAL_BLOCK
		- Create functions to resize program and user space
*/
void gt_init() {
	
	pthread_mutex_init(&allocmutex,NULL);
	pthread_mutex_init(&freemutex,NULL);	

	usr_mem = mmap(NULL, INITIAL_BLOCK, PROT_READ | PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
	if(usr_mem == MAP_FAILED){
		//print error and errno, then die?
		//printf("ERROR: User Space mmap failed\n");
		exit(0);
	}
	printf("user mems = %p\n", usr_mem);
	size_t prg_space;
	prg_space = calc_prg_mem_size(MINIMUM_BLOCK, INITIAL_BLOCK);
	

	//prg_mem layout is as follows
	//a map struct that points to the next map (for expanding)
	//an array of block_lists for the allowed block sizes
	//an array of blocks
	//may need to add an array for freeing

	prg_mem = mmap(NULL, prg_space, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS,-1,0);
	printf("prg_mem + prg_space = %p\n", prg_mem + prg_space);
	if(prg_mem == MAP_FAILED){
		//print error and errno, then die
		//printf("ERROR: Program Space mmap failed\n");
		exit(0);
	}
	memset(prg_mem, 0, prg_space);
	
	num_sizes = ((log2(INITIAL_BLOCK) - log2(MINIMUM_BLOCK))+1);
	//printf("num of sizes = %i\n", num_sizes);
	first_map = (map *) prg_mem;
	curr_map = first_map;
	printf("map size is %x\n", (uint32_t) sizeof(map));
	printf("block_list size is %x\n", (uint32_t) sizeof(block_list));
	printf("first_map = %p\n", first_map);
	first_map->head = prg_mem + sizeof(map);
	printf("old head = %p\n", first_map->head);
	first_map->head = (block_list *) &(first_map[1]);
	printf("first_map->head = %p\n", first_map->head);
	printf("head offset %x\n", (uint32_t)((void *)first_map->head - (void *)first_map));
	first_map->free_list = prg_mem + sizeof(map) +				
					(sizeof(block_list) * num_sizes);
	printf("first_map->free_list = %p\n", first_map->free_list);
	printf("free_list offset %x\n", (uint32_t)((void *)first_map->free_list - (void *)first_map));
	first_map->alloced_list = prg_mem + sizeof(map) + 
					   (sizeof(block_list) * num_sizes) +
					   (sizeof(block) * FREE_ARRAY(num_sizes,0));
    	printf("first_map->alloced_list = %p\n", first_map->alloced_list);
	printf("alloced_list offset %x\n", (uint32_t)((void *)first_map->alloced_list - (void *)first_map));
	first_map->alloced_list->alloc_head = prg_mem + sizeof(map) + 
					    sizeof(rl_lists) +
					   (sizeof(block_list) * num_sizes) +
					   (sizeof(block) * FREE_ARRAY(num_sizes,0));	
	printf("first_map->alloced_list->alloc_head = %p\n", first_map->alloced_list->alloc_head);
	printf("alloc_head offset %x\n", (uint32_t)((void *)first_map->alloced_list->alloc_head - (void *)first_map));
	first_map->alloced_list->alloc_tail = first_map->alloced_list->alloc_head;
	//printf("first_map->alloced_list->alloc_tail = %p\n", first_map->alloced_list->alloc_tail);
	first_map->alloced_list->first_open_node = first_map->alloced_list->alloc_head;
	//printf("first_map->alloced_list->afirst_open_node = %p\n", first_map->alloced_list->first_open_node);
	printf("prg_space = %x\n", (uint32_t) prg_space);


	size_t block_size = INITIAL_BLOCK;
	curr_list = first_map->head;
	block *curr_block = first_map->free_list;
	int offset = 1;

	//initalize first block
	curr_list->size = block_size;
	curr_list->location_array = curr_block;
	curr_list->array_size = offset;
	curr_list->location_array->location = usr_mem;
	curr_list->location_array->free = FREE;
	curr_list->location_array->buddy = NULL;
	
	printf("Size of block = %x\n", (uint32_t) sizeof(block));
	printf("Size of rl_list = %x\n", (uint32_t) sizeof(rl_lists));
	printf("Size of rl_node = %x\n", (uint32_t) sizeof(rl_node));


	//initialize the rest
	int i = 0;
	void *location = curr_list->location_array;
	for(i = 1; i < num_sizes; i++) {
		block_size = block_size/2;
		curr_list[i].size = block_size;
		//printf("curr_list[%i].location_array = %p\n", i-1, curr_list[i-1].location_array);
		//printf("offset = %x", offset);
		//printf("size = %x\n", (offset * sizeof(block)));
		location = location + (offset * sizeof(block));
		curr_list[i].location_array = location;
		//printf("curr_list[%i].location_array = %p\n", i, curr_list[i].location_array);
		offset = offset * 2;
		curr_list[i].array_size = offset;
	}
	
	//mem_dump();
}

__attribute__ ((destructor)) void destroy() {
	// unmap all the mmapped regions
}

void mem_dump(){
	int i = 0, j = 0;
	
	for(i = 0; i < num_sizes; i++) {
		printf("printing for order = %i\n", i);
		for(j = 0; j < curr_list[i].array_size; j++){
			printf("index = %i -- free? = %i\n", j, curr_list[i].location_array[j].free);
			
		}
	}
}

void * gtalloc(size_t bytes){
	/*
		TODO:
			- Implement best-fit algorithm to find and assign memory
			- Update memory tree
			- Log user allocation
			- Return usable pointer to user
	*/ 
    pthread_mutex_lock(&allocmutex);
    if (bytes == 0) {
		return NULL;
    }
    map *comp_map = first_map;
    while(bytes > comp_map->head->size){
	if(comp_map->next_map !=NULL){
		comp_map = comp_map->next_map;
	}
	else{
		map *newGuy;
		//mapinit(newGuy)
	}
    }
    size_t order = get_requested_order(bytes);
    //printf("looking for free block big enough -- order = %u\n", order);
    int index = find_free(order);
    //printf("find_free returns %i\n", index);
    if(index == -1){
	    	//printf("couldn't find one. making one\n");
	    	void *ret = split(order);
    		return ret;
    } else {
    	printf("index = %i\n", index);	
    	add_alloc(order, index);
    	curr_list[order].location_array[index].free = UNAVAILABLE;
		return ((void *) curr_list[order].location_array[index].location);
    }
    mem_dump(); 
    return NULL;
    pthread_mutex_unlock(&allocmutex);
}

void gtfree(void *addr){
	/*
		TODO:
			- Reverse lookup block by address
			- Mark unallocated block as free
				(recursively)
			- Check buddy partitions
			- Merge if buddy is currently free
	*/
	pthread_mutex_lock(&freemutex);
	printf("Starting gtfree(%p)\n", addr);
	rl_node *curr_node = first_map->alloced_list->alloc_head;
	//printf("assign curr_node\n");
	while(curr_node->location != addr && curr_node->location != NULL) {
		//printf("curr_node->location = %p\n", curr_node->location);
		curr_node = curr_node->next;
	}
	fflush(stdout);
	block *block = curr_node->alloced_block;
	block->free = FREE;
	printf("Block freed, starting merge\n");
	merge(curr_node);
	printf("merge done, starting remove_alloc\n");
	remove_alloc(curr_node);
	pthread_mutex_unlock(&freemutex);
}

int find_free(size_t order){
	int i = 0;
	block *curr = curr_list[order].location_array;
	//printf("array size for order %u --> %i\n", order, curr_list[order].array_size);
	for(i = 0; i < curr_list[order].array_size; i++){
		//printf("i is free? %i\n", curr[i].free);
		if(curr[i].free == FREE){
			return i;
		}
	}
	return -1;
}

size_t calc_prg_mem_size(int min_block, int total_block) {
	int total_size;	
	int num_sizes = (log2(total_block) - log2(min_block));
	total_size = sizeof(map);
	printf("map %x\n", total_size);
	total_size = total_size + (sizeof(block_list) * (num_sizes + 1));
	printf("map %x\n", total_size);
    	total_size = total_size + (sizeof(block) * FREE_ARRAY((num_sizes + 1),0));
	printf("map %x\n", total_size);
	total_size = total_size + sizeof(rl_lists);
	printf("map %x\n", total_size);
	total_size = total_size + (sizeof(rl_node) * (total_block / min_block));
	printf("map %x\n", total_size);
	return (size_t) total_size;
}

size_t get_requested_order(size_t bytes){
	size_t order = 10;
	int size = MINIMUM_BLOCK;
	while(bytes > size){
		order--;
		size = size * 2;
	}
	return order;
}

//add adds a block to alloced list
void add_alloc(size_t order, int offset) {
	rl_lists *list = first_map->alloced_list;
	rl_node *node = list->first_open_node;
	int rl_size = (uint32_t) sizeof(rl_node);
	printf("rl_node size is %x\n", rl_size);
	node->alloced_block = &(curr_list[order].location_array[offset]);
	printf("block addr = %p\n", node->alloced_block);
	node->location = node->alloced_block->location;
	printf("alloc addr = %p\n", node->location);
	node->order = order;
	printf("node order set to %i\n", (int) order);
	node->offset = offset;
	printf("node offset set to %i\n", offset);
	if (list->first_open_node == list->alloc_tail) {
		printf("add_alloc's in if\n");
		printf("nodes address is %p\n", node);
		node->next = &(node[1]);
		printf("node now points to %p\n", node->next);
		list->alloc_tail = node->next;
		printf("moved the tail pointer to %p\n", list->alloc_tail);
		list->first_open_node = list->alloc_tail;
		printf("first open node pointer is now %p\n", list->first_open_node);	
	} else {
		printf("add_alloc's in else\n");
		node->next = list->first_open_node;
		list->first_open_node = list->first_open_node->next;
	}
	printf("made it out of the ifs\n");
	node->next->prev = node;
	printf("set new nodes prev to %p\n", node);
}


//remove will remove from alloced_list
void remove_alloc(rl_node *node) {
	rl_lists *list = first_map->alloced_list;
	node->location=NULL;
	if (node == list->alloc_head) {
		list->alloc_head = node->next;
		node->next->prev = NULL;
	} else if (node->next == list->alloc_tail) {
		list->alloc_tail = node;
	} else {
		node->prev->next = node->next;
		node->next->prev = node->prev;
	}
	node->next = list->first_open_node;
	list->first_open_node->prev = node;
	list->first_open_node = node;
	node->prev = NULL;
}


void * split(size_t order) {
	int i = 0;
	int offset;
	//printf("Starting split(%u) -- ", order);
	fflush(stdout);
	do {
		i++;
		offset = find_free(order - i);
		//printf("find_free(%i) => %i\n", order - i, offset);
		fflush(stdout);
	} while (offset == -1);
	//printf("Got offset = %i -- ", offset);
	fflush(stdout);
	block *curr_block = &(curr_list[order - i].location_array[offset]);
	void *temp_loc = curr_block->location;
	curr_block->free = UNAVAILABLE;
	do {
		i--;
		offset = offset * 2;
		curr_block = &(curr_list[order - i].location_array[offset]);
		curr_block->free = UNAVAILABLE;
		curr_block->location = temp_loc;
		curr_block->buddy = &(curr_list[order - i].location_array[(offset + 1)]);
		curr_block->buddy->free = FREE;
		curr_block->buddy->location = temp_loc	+ curr_list[order - i].size;
	    curr_block->buddy->buddy = curr_block;
	} while (i > 0);
	add_alloc(order, offset);
	return temp_loc;
}

void merge(rl_node *node) {
	int order = node->order;
	printf("order = %i\n", order);
	int offset = node->offset;
	printf("offset = %i\n", offset);
	block *curr_node = node->alloced_block;
	while (order > 0 && curr_node->buddy->free == FREE) {
		curr_node->free = UNAVAILABLE;
		curr_node->buddy->free = UNAVAILABLE;
		order--;
		offset = offset / 2;
		curr_node = &(curr_list[order].location_array[offset]);
	    curr_node->free = FREE;	
	}
}
