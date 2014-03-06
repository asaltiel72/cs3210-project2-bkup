#ifndef _GTALLOCATOR_H_
#define _GTALLOCATOR_H_
#endif

#include <math.h>

// Size (in bytes) of initial or new mmapped memory chucks
#define INITIAL_BLOCK 16384		// 16 KB
#define MINIMUM_BLOCK 16		// 16 bytes
#define PRG_SPACE (INITIAL_BLOCK/MINIMUM_BLOCK)*sizeof(node)

//state values
#define UNINITIALIZED 0
#define INITIALIZED 1
#define NONE_FREE 2

//free values
#define FREE 1
#define TAKEN 0

#define FREE_ARRAY(i, j) (((-2 + pow(2, (i+1)))/2) + j)

/*
	TODO: Outline what thread locks are needed
		- I think we should lock almost everything
*/

typedef struct _block {
	int free;
	uint32_t location;
	_block *buddy;
} block;

typedef struct {
	uint32_t size;
	block *location_array;
	int array_size;	
} block_list;

typedef struct _map {
	struct block_list *head;
	struct block *free_list;
	struct _map *next_map;
} map;


// global variable to store allocated memory
void *usr_mem;
void *prg_mem;
map *first_map;
block_list *curr_list;
int num_sizes;


void * gtalloc(size_t bytes);

void gtfree(void * addr);
