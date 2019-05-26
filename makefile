decoding: decoding.c decoding_impl.c
	gcc -g3 -O3 -Wall -o decoding_bits decoding.c decoding_impl.c -std=c99
