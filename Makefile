CC=gcc

gtallocator: gtallocator.c
	$(CC) -c -Wall -fpic gtallocator.c -lm
	$(CC) -shared -o libgtmem.so gtallocator.o -lm
	sudo cp libgtmem.so /usr/lib
	$(CC) -o memtest memtest.c -lm -lgtmem

clean: 
	-rm gtallocator.o libgtmem.so memtest
