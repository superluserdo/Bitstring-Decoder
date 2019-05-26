/* Edit this file to specify the data format in bits. */

enum field_e {
	field_1,
	field_2,
	field_3,
	field_4,
	field_5,
	field_6,
	field_7,
	field_8,
	field_9,
	field_10,
	field_11,
	field_12,
	ALL_FIELDS,
};

struct pos_len_s fields_pos_len[ALL_FIELDS] = {
	/* Start bit, length, endianness */
	{0  , 64, LE},  //field_1
	{64 , 32, LE},  //field_2
	{96 , 1 , LE},  //field_3
	{97 , 1 , LE},  //field_4
	{98 , 1 , LE},  //field_5
	{99 , 1 , LE},  //field_6
	{100, 32, BE},  //field_7
	{132, 4 , LE},  //field_8
	{136, 30, LE},  //field_9
	{166, 30, LE},  //field_10
	{196, 30, LE},  //field_11
	{226, 30, LE},  //field_12
};
