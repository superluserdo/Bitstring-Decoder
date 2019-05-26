#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <byteswap.h>
#include "decoding.h"
#include "decoding_config.h"

const int linelen_bytes = 32;

unsigned char linebuf[32];

int main(int argc, char **argv) {
	masks_init();
	assert(argc >= 3);
	int num_lines = atoi(argv[1]);
	fprintf(stderr, "Reading %d lines from file: %s\n", num_lines, argv[2]);
	FILE *file_read = fopen(argv[2], "rb");
	FILE *file_write = fopen("test_write_c.csv", "w");

	/* Skip header */
	fseek(file_read, 27*32/8, SEEK_CUR);

	for (int line = 0; line < num_lines || num_lines < 0; line++) {
		if (line%256 == 0 && line > 0) {
			fseek(file_read, 6*32/8, SEEK_CUR);
		}

		int rc = fread(linebuf,linelen_bytes,1,file_read);
		if (!rc) {
			fprintf(stderr, "Finished reading file after %d lines\n", line);
			break;
		}

		struct bits64_len line_data[ALL_FIELDS];
		decode_line(linebuf, line_data, fields_pos_len, ALL_FIELDS);
		write_line(line_data, ALL_FIELDS);
	}

	fclose(file_read);
	fclose(file_write);
}
