#define SPP_PER_LINE 4

enum endian_e {BE, LE};
	
union bits64 {
	uint64_t integer;
	unsigned char bytes[8];
};

struct bits64_len {
	union {
		uint64_t integer;
		unsigned char bytes[8];
	};
	int len_bytes;
	enum endian_e endianness;
};

enum align_e {LEFT, RIGHT};
	
struct pos_len_s {
	int pos;
	int len;
	enum endian_e endianness;
};

int is_LE();

long int make_native_int(struct bits64_len data);

void masks_init();

int byte_first_n_bits(unsigned char byte, int n, enum align_e alignment);

int byte_bit_range(unsigned char byte, int n_start, int n_end, enum align_e alignment);

struct bits64_len chars_to_long(unsigned char *bytes, int n_bytes, int start_bits, int end_bits, enum endian_e endianness, int offset);

struct bits64_len bits_to_long(unsigned char *bytes, int bitstart, int bitlen, enum endian_e endianness);

void decode_line(unsigned char *bytes, struct bits64_len line_data[], struct pos_len_s fields_pos_len[], int num_fields);

void write_line(struct bits64_len line_data[], int num_fields);
