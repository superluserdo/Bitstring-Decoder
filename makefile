decoding_lib: decoding.c decoding.h
	gcc -g3 -O3 -Wall -c decoding.c -std=c99

clean:
	rm *.o 
