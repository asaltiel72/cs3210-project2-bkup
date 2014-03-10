CC=gcc

gtallocator: gtallocator.c
	$(CC) -c -Wall -fpic gtallocator.c -lm
	$(CC) -shared -o libgtmem.so gtallocator.o -lm
	$(CC) -o memtest memtest.c -lm -L/home/alex/Desktop/CS3210/cs3210_project2 -lgtmem

clean: 
	-rm gtallocator.o libgtmem.so memtest
