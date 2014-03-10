CC=gcc

gtallocator: gtallocator.c
	$(CC) -c -Wall -fpic gtallocator.c -lm
	$(CC) -shared -o libgtmem.so gtallocator.o
	$(CC) -o memtest memtest.c -lm -L/home/j3gb3rt/Documents/cs3210/proj2/cs3210_project2 -lgtmem

clean: 
	-rm gtallocator.o libgtmem.so memtest
