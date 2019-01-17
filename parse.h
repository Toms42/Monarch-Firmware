#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_PACKET_SIZE (256)

typedef enum
{
	TAG_CONT = 0,
	TAG_CONFIG,
	TAG_THROTTLE,
	TAG_MEMES,
	TAG_UNKNOWN,
	NUM_TAG
}tag_type_t;

int tag_size_LUT[NUM_TAG] = {3, 4, 5, 5, 5};

typedef struct{
	tag_type_t tag_type;
	int tag_values[256];
	int tag_values_size;
}tag_t;

int decode(char *chars, long *vals);
tag_type_t get_tag_type(char *chars, int *value_start);
tag_t decode_tag(char *chars);
