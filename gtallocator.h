#ifndef _GT_ALLOC
#define _GT_ALLOC
#endif

// Size (in bytes) of initial or new mmapped memory chucks
// Example on wikipedia is 1024 KB
#define INITIAL_BLOCK 1048576

// Defined stats for a tree node
#define EMPTY 0
#define PARTIALLY_OCCUPIED 1
#define OCCUPIED 2

/*
	TODO: Outline what thread locks are needed
		- I think we should lock almost everything
*/

struct node {
	uint32_t size;
	struct node *next;
};

// global variable to store allocated memory
extern void *user_mem;
extern void *prg_mem;

void * gtalloc(size_t bytes);

void gtfree(void * addr);
