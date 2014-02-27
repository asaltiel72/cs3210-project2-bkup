CC=gcc

gtallocator: gtallocator.o
	$(CC) -o gtallocator gtallocator.c

clean: 
	-rm gtallocator.o
