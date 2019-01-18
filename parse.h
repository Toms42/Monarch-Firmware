/*
 * parse.h
 *
 *  Created on: 1/17/2019
 *      Author: Advaith Sethuraman
 */

#ifndef PARSE_H_
#define PARSE_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "uart.h"

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

typedef struct{
	tag_type_t tag_type;
	int tag_values[256];
	int tag_values_size;
}tag_t;


/*returns a struct with tag_type, tag_values, and tag_values_size*/
tag_t decode_tag(char *chars);
void send_to_UART(tag_t test_struct);

#endif
