#ifndef _GTALLOCATOR_H_
#define _GTALLOCATOR_H_
#endif

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <stdint.h>
#include <sys/mman.h>

// Size (in bytes) of initial or new mmapped memory chucks
#define INITIAL_BLOCK 16384		// 16 KB
#define MINIMUM_BLOCK 16		// 16 bytes
#define PRG_SPACE (INITIAL_BLOCK/MINIMUM_BLOCK)*sizeof(node)

//free values
#define FREE 1
#define TAKEN 0
//indexed at 0
#define FREE_ARRAY(i, j) (uint32_t) (((-2 + pow(2, (i+1)))/2) + j)

/*
	TODO: Outline what thread locks are needed
		- I think we should lock almost everything
*/

typedef struct _block {
	int free;
	void *location;
	struct _block *buddy;
} block;

typedef struct {
	size_t size;
	block *location_array;
	int array_size;
	int num_elements;
} block_list;

//Reverse lookup node
typedef struct _rl_node {
	void *location;
	block *alloced_block;
	struct _rl_node *next;
	struct _rl_node *prev;
} rl_node;

//Reverse lookup list
//last node can be found by
//location == NULL
typedef struct _rl_lists {
	rl_node *alloc_head;
	rl_node *alloc_tail;
	rl_node *first_open_node;
} rl_lists;

typedef struct _map {
	block_list *head;
	block *free_list;
	rl_lists *alloced_list;
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

