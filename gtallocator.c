#include <stdlib.h>
#include <unistd.h> //for brk & sbrk


void * gtalloc(size_t bytes){
    if (bytes == 0) {
	return NULL;
    }
}

void gtfree(void * addr){

}
