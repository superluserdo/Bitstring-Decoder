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

int is_LE(void);

long int make_native_int(struct bits64_len data);

void decoder_init(void);

void masks_init(void);

int byte_first_n_bits(unsigned char byte, int n, enum align_e alignment);

int byte_bit_range(unsigned char byte, int n_start, int n_end, enum align_e alignment);

struct bits64_len chars_to_long(unsigned char *bytes, int n_bytes, int start_bits, int end_bits, enum endian_e endianness, int offset);

struct bits64_len bits_to_long(unsigned char *bytes, int bitstart, int bitlen, enum endian_e endianness);

int bits_src_to_dst(unsigned char *bytes_src, int bitstart_src, int bitlen_src, enum endian_e endianness_src,
		unsigned char *bytes_dst, int bitstart_dst, int bitlen_dst, enum endian_e endianness_dst);

struct bits64_len decode_pos(unsigned char *bytes, struct pos_len_s pos_len, int offset_bits);

void decode_line(unsigned char *bytes, struct bits64_len line_data[], struct pos_len_s fields_pos_len[], int num_fields, int offset_bits);

void write_line(struct bits64_len line_data[], int num_fields);

void decoder_hex2bytes(const char *hexstring, unsigned char *bytestring, int *len_bytestring);
int decoder_linestartswith(unsigned char *linebuf, int len_linebuf, unsigned char *bytestring, int len_bytestring);
