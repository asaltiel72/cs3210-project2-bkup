#ifndef _GT_ALLOC
#define _GT_ALLOC
#endif

// Size (in bytes) of initial or new mmapped memory chucks
#define INITIAL_BLOCK 1048576	// 1024 KB
#define MINIMUM_BLOCK 1024		// 1 KB

// Defined stats for a tree node
#define EMPTY 0					// Completely unallocated, no child leaves
#define PARTIALLY_OCCUPIED 1	// at least part of 1 child leaf allocated 
#define OCCUPIED 2				// both child leaves occupied

/*
	TODO: Outline what thread locks are needed
		- I think we should lock almost everything
*/

typedef struct _node {
	uint32_t size;
	uint32_t *location_array;
	struct node *previous;
	struct node *next;
} node;

//pointer to head of tree
struct node *head;

// global variable to store allocated memory
extern void *user_mem;
extern void *prg_mem;

void * gtalloc(size_t bytes);

void gtfree(void * addr);
