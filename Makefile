CC=gcc

gtallocator: gtallocator.c
	$(CC) -c -Wall -Werror -fpic gtallocator.c
	$(CC) -shared -o libgtmem.so gtallocator.o

clean: 
	-rm gtallocator.o libgtmem.so
