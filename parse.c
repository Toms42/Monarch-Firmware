
#include "parse.h"
#include "uart.h"


const int tag_size_LUT[NUM_TAG] = {2, 4, 5, 5, 5};

/*takes numbers after colon and puts it into a long array*/
int decode(char *chars, long *vals)
{
	return 0;

}

void send_to_UART(tag_t test_struct)
{
    uart_printstring("tag_type:");
    uart_printintln((int)test_struct.tag_type);
    uart_printstringln("values:");
    int iter;
    for(iter = 0; iter <  test_struct.tag_values_size; iter++)
      {
         uart_printintln((int)test_struct.tag_values[iter]);
      }
}
/*function determine tag from enum and hardcoded string values*/
tag_type_t determine_tag(char *buf)
{
	if(!strcmp(buf, "TAG_CONT")) return TAG_CONT;
	else if(!strcmp(buf, "TAG_CONFIG")) return TAG_CONFIG;
	else if (!strcmp(buf, "TAG_THROTTLE")) return TAG_THROTTLE;
	else if (!strcmp(buf, "TAG_MEMES")) return TAG_MEMES;
	else return TAG_UNKNOWN;
}

tag_type_t get_tag_type(char *chars, int *value_start)
{
	char buf[MAX_PACKET_SIZE];
	int i = 0;
	while(chars[i] != (char)':' && chars[i] != (char)';' && chars[i] != (char)'\0' && i < MAX_PACKET_SIZE)
	{
		buf[i] = chars[i];
		//printf("i:%d character: %c\n", i, buf[i]);
		i++;
		
	}

	buf[i] = (char)'\0';
	*value_start = i+1; //give char position past colon
	//uart_printstringln(buf);
	return determine_tag(buf);

}


void printf_array(int *arr, int size)
{
    int i;
	for(i = 0; i < size; i++)
	{
		printf("index %d: %d\n", i, arr[i]);
	}
}

/*Takes stream of strings and returns struct with tag enum, int array of values */ 
tag_t decode_tag(char *chars)
{
	tag_t tag;
	int value_start;
	tag_type_t tag_type = get_tag_type(chars, &value_start);
	//printf("Tag type: %d\n", tag_type);
	chars = chars + value_start;
	int tag_values_arr_size = tag_size_LUT[tag_type];
	char temp_buf[MAX_PACKET_SIZE];
	
	if(tag_type < NUM_TAG)
	{
		//printf("enter\n");
		int i; 
		int left = 0;
		int right = 0;

		for(i = 0; i < tag_values_arr_size; i++)
		{
			
			while(chars[left+right] != (char)',' && chars[left+right] != (char)';' && chars[left+right] != (char)'\0' \
					 &&  right < MAX_PACKET_SIZE && left + right < MAX_PACKET_SIZE)
			{
				temp_buf[right] = chars[left+right];
				//printf("i:%d character: %c, left: %d, right:%d\n", i, temp_buf[right], left, right);
				right++;
			}
			//buffer is full, bring left to right
			left = left + right + 1;
			right = 0;

			tag.tag_values[i] = atoi(temp_buf);
			memset(temp_buf, 0x00, MAX_PACKET_SIZE); //reset buffer
			//printf("tag values: %d\n", tag_values[i]);
		}
		tag.tag_type = tag_type;
		tag.tag_values_size = tag_values_arr_size;
	}
	
	//printf("tag struct:\n tag_type: %d\n", tag.tag_type);
	//printf_array(tag.tag_values, tag.tag_values_size);

	return tag;

}

/*
int main()
{
	int value_start;
	char *s = "TAG_CONT:-1574,1645,-6969;\r\n"; //test string 1
	char *s2 = "TAG_CONFIG: 1234,1235,-1236,12375;\r\n";
	char *malformed = "TAG_MEMES:69696,69696,23,6,7;\r\n";
	//printf("Tag type: %d\n", get_tag_type(s, &value_start));
	//printf("value start: %d\n", value_start);
	//decode_tag(s);
	//decode_tag(s2);
	tag_t malformed_t = decode_tag(s);
	printf("\nmalformed type: %d\n", malformed_t.tag_type);
	printf_array(malformed_t.tag_values, malformed_t.tag_values_size);
	tag_t reg_t = decode_tag(s2);
	printf("\ntag type: %d\n", reg_t.tag_type);
	printf_array(reg_t.tag_values, reg_t.tag_values_size);
	return 0;
}*/
