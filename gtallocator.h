#ifndef _GTALLOCATOR_H_
#define _GTALLOCATOR_H_
#endif

#include <math.h>

// Size (in bytes) of initial or new mmapped memory chucks
#define INITIAL_BLOCK 1048576	// 1024 KB
#define MINIMUM_BLOCK 64	// 64 bytes
#define PRG_SPACE (INITIAL_BLOCK/MINIMUM_BLOCK)*sizeof(node)

//state values
#define UNINITIALIZED 0
#define INITIALIZED 1
#define FULL 2

//free values
#define FREE 1
#define TAKEN 0

#define FREE_ARRAY(i, j) (((-2 + pow(2, (i+1)))/2) + j)

/*
	TODO: Outline what thread locks are needed
		- I think we should lock almost everything
*/

typedef struct _free_addr {
	int free = TAKEN;
	uint32_t location;
	free_addr *buddy;
} free_addr;

typedef struct {
	struct free_addr *location_array;
	int state = UNINITIALIZED;
	uint32_t size;
} node;

typedef struct {
	struct node *head;
	struct free_addr *free_list;
	struct map *next_map;
} map;


// global variable to store allocated memory
void *user_mem;
void *prg_mem;

void * gtalloc(size_t bytes);

void gtfree(void * addr);
