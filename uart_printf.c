/*!
 * \file uart_printf.c
 * \brief Driver designed to create print functionality over UART without the need for stdio.h.
 *
 * \author Tom Scherlis
 * \date 2/18/2018
 *
 * Edit log: 12/22/2018 Ported to MSP430 and increased digit size for printint from 0 to 1
 *
 */

#include "uart_printf.h"
#include <string.h>
#include "math.h"

void reverse(char *str, int len)
{
    int i=0, j=len-1, temp;
    while (i<j)
    {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++; j--;
    }
}

int intToStr(int x, char str[], int d)
{
    uint8_t negative = 0;

    if(x < 0)
    {
        negative = 1;
        x = -x;
    }

    int i = 0;
    while (x)
    {
        str[i++] = (x%10) + '0';
        x = x/10;
    }

    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < d)
        str[i++] = '0';

    if(negative)
    {
        str[i++] = '-';
    }
    reverse(str, i);
    str[i] = '\0';
    return i;
}

void uart_printfloat (float n, int afterpoint)
{
    char res[32];    // Extract integer part
    int ipart = (int)n;

    // Extract floating part
    float fpart = n - (float)ipart;
    uint16_t i = 0;

    if(n < 0)
    {
        i = 1;
        n = -n;
    }

    if(i==1)
    {
        res[0] = '-';
    }
    // convert integer part to string
    i += intToStr(abs(ipart), res + i, 0);

    // check for display option after point
    if (afterpoint != 0)
    {
        res[i] = '.';  // add dot

        // Get the value of fraction part upto given no.
        // of points after dot. The third parameter is needed
        // to handle cases like 233.007
        fpart = fpart * pow(10, afterpoint);

        intToStr(abs((int)fpart), res + i + 1, afterpoint);
    }
    uart_printstring(res);
}

void uart_printint(int num)
{
    char res[32];
    intToStr(num, res, 1);
    uart_printstring(res);
}

void uart_printstring(char *string)
{
    uart_puts(string, (uint32_t) strlen(string));
}


void uart_printintln(int num)
{
    uart_printint(num);
    uart_printstring("\n\r");
}

void uart_printstringln(char* string)
{
    uart_printstring(string);
    uart_printstring("\n\r");
}

void uart_printfloatln(float num, int afterpoint)
{
    uart_printfloat(num,afterpoint);
    uart_printstring("\n\r");
}
