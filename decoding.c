#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <byteswap.h>
#include "decoding.h"

unsigned char linebuf[32];

char masks_first[9];
char masks_last[9];

int is_LE(void) {
	/* Test if this PC is little-endian */
	union bits64 testint = {.integer = 1};
	if (testint.bytes[0] == 1) {
		return 1;
	} else {
		return 0;
	}
}

long int make_native_int(struct bits64_len data) {
	if ((data.endianness == LE) != is_LE()) {
		return bswap_64(data.integer);
	} else {
		return data.integer;
	}
}

void decoder_init(void) {
	masks_init();
}

void masks_init(void) {
	char mask_first = 0;
	char mask_last = 0;
	masks_first[0] = mask_first;
	masks_last[0] = mask_last;
	for (int i = 0; i < 8; i++) {
		mask_first += 1 << (7-i);
		masks_first[i+1] = mask_first;
		mask_last += 1 << i;
		masks_last[i+1] = mask_last;
	}
}

int byte_first_n_bits(unsigned char byte, int n, enum align_e alignment) {
	/* Get the first n bits of a char */
	unsigned int masked_byte = byte & masks_first[n];
	if (alignment == RIGHT) {
		masked_byte = masked_byte >> (8-n);
	}
	return masked_byte;
}
	
int byte_last_n_bits(unsigned char byte, int n, enum align_e alignment) {
	/* Get the last n bits of a char */
	unsigned char masked_byte = byte & masks_last[n];
	if (alignment == LEFT) {
		masked_byte = masked_byte << (8-n);
	}
	return masked_byte;
	
}

int byte_bit_range(unsigned char byte, int n_start, int n_end, enum align_e alignment) {
	/* Get the first n_start bits AND last n_end bits of a char */
	unsigned char masked_byte = byte & masks_first[n_start] & masks_last[n_end];
	if (alignment == LEFT) {
		masked_byte = masked_byte << (8-n_end);
	} else {
		masked_byte = masked_byte >> (8-n_start);
	}
	return masked_byte;
	
}

struct bits64_len chars_to_long(unsigned char *bytes, int n_bytes, int start_bits, int end_bits, enum endian_e endianness, int offset) {
	assert(n_bytes * 8 + end_bits <= 64); //Make sure end result < 64 bits
	int empty_bytes = 8 - n_bytes;

	struct bits64_len result = {.len_bytes = n_bytes, .integer = 0, .endianness = endianness};

	for (int i = 0; i < n_bytes; i++) {
		if (endianness == LE) {
			unsigned char shift_remainder = bytes[i] >> start_bits;
			unsigned char shift_carry = bytes[i] << (8 - start_bits);
			result.bytes[i+offset] += shift_remainder;
			if (shift_carry) {
				if (i == n_bytes-1) {
					result.len_bytes++;
				}
				result.bytes[i+offset+1] += shift_carry;
			}
		} else {
			unsigned char shift_remainder = bytes[i] << end_bits;
			unsigned char shift_carry = bytes[i] >> (8 - end_bits);
			if (shift_carry) {
				if (i == 0) {
					result.len_bytes++;
				}
				result.bytes[empty_bytes+i-offset-1] += shift_carry;
			}
			result.bytes[empty_bytes+i-offset] += shift_remainder;
		}
	}
	return result;
}

struct bits64_len bits_to_long(unsigned char *bytes, int bitstart, int bitlen, enum endian_e endianness) {
	int bitend = bitstart + bitlen; //Exclusive end of bits

	int startbyte = bitstart/8;
	int startbyte_lastbits = 8 - (bitstart % 8);
	/* Leave full bytes to the midbytes -- don't write 8 bits here */
	startbyte_lastbits = startbyte_lastbits % 8;

	int endbyte = bitend/8;
	int endbyte_firstbits = (bitlen > startbyte_lastbits) ? bitend % 8 : 0;

	int midbytes_start = (bitstart%8) ? startbyte+1: startbyte;
	int midbytes_len = (endbyte > midbytes_start) ? endbyte - midbytes_start : 0;

	enum align_e alignment = (midbytes_len > 0 && endianness == LE)? LEFT : RIGHT;

	uint64_t start_char;
	
	if (bitlen < startbyte_lastbits) {
		start_char = byte_bit_range(bytes[startbyte], (8-startbyte_lastbits+bitlen), startbyte_lastbits, alignment);
	} else {
		start_char = byte_last_n_bits(bytes[startbyte], startbyte_lastbits, alignment);
	}
	
	uint64_t end_char = byte_first_n_bits(bytes[endbyte], endbyte_firstbits, alignment);

	unsigned char start_remainder;
	unsigned char start_carry;
	unsigned char end_remainder;
	unsigned char end_carry;

	if (alignment == LEFT) {
		start_remainder = start_char;
		start_carry = 0;
		end_remainder = end_char >> startbyte_lastbits;
		end_carry = end_char << (8 - startbyte_lastbits);
	} else {
		start_remainder = start_char << endbyte_firstbits;
		start_carry = start_char >> (8 - endbyte_firstbits);
		end_remainder = end_char;
		end_carry = 0;
	}

	int offset = 0;
	if ((endianness == LE && start_carry) ||
		(endianness == BE && end_carry)) {
		offset = 1;
	}

	struct bits64_len result = chars_to_long(&bytes[midbytes_start], midbytes_len, startbyte_lastbits, endbyte_firstbits, endianness, offset);

	int index = (result.len_bytes > 0) ? result.len_bytes - 1 : 0;
	if (endianness == LE) {
		result.bytes[0] += start_carry;
		result.bytes[0+offset] += start_remainder;
		result.bytes[index+offset] += end_remainder;
		result.bytes[index+1+offset] += end_carry;
	} else {
		result.bytes[7] += end_carry;
		result.bytes[7-offset] += end_remainder;
		result.bytes[7-index-offset] += start_remainder;
		result.bytes[7-index-1-offset] += start_carry;
	}

	return result;
}
		
struct bits64_len decode_pos(unsigned char *bytes, struct pos_len_s pos_len) {
	return bits_to_long(bytes, pos_len.pos, pos_len.len, pos_len.endianness);
}
	

void decode_line(unsigned char *bytes, struct bits64_len line_data[], struct pos_len_s fields_pos_len[], int num_fields) {

	for (int i = 0; i < num_fields; i++) {
		line_data[i] = decode_pos(bytes, fields_pos_len[i]);
	}
}

void write_line(struct bits64_len line_data[], int num_fields) {

	/* Write to CSV */
	for (int i = 0; i < num_fields; i++) {
		fprintf(stdout, "%ld, ", make_native_int(line_data[i]));
	}
	fprintf(stdout, "\n");
}
