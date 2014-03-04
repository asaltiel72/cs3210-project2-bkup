#ifndef _GT_ALLOC
#define _GT_ALLOC
#endif

// Size (in bytes) of initial or new mmapped memory chucks
#define INITIAL_BLOCK 1048576	// 1024 KB
#define MINIMUM_BLOCK 64	// 64 bytes
#define PRG_SPACE (INITIAL_BLOCK/MINIMUM_BLOCK)*sizeof(node)

/*
	TODO: Outline what thread locks are needed
		- I think we should lock almost everything
*/

typedef struct _free_addr {
	uint32_t location;
	struct free_addr *next;
} free_addr;

typedef struct {
	//struct free_addr *location_array;
	int is_available;
	uint32_t size;
	struct node *buddy;
	struct node *previous;
	struct node *next;
} node;

//pointer to head of list
struct node *head;

// global variable to store allocated memory
void *user_mem;
void *prg_mem;

void * gtalloc(size_t bytes);

void gtfree(void * addr);
