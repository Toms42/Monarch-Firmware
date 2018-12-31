/*!
 * \file uart_printf.h
 * \brief Driver designed to create print functionality over UART without the need for stdio.h.
 *
 * \author Tom Scherlis
 * \date 2/18/2018
 */

#include "uart.h"

/*! \brief prints a string over uart
 *  \param string [in] string to print
 */
void uart_printstring(char *string);

/*! \brief prints an int over uart
 *  \param num [in] int to print. Supports negative/positive ints.
 */
void uart_printint(int num);

/*! \brief prints a float over uart
 *  \param string [in] float to print. Supports negative/positive floats.
 */
void uart_printfloat (float n, int afterpoint);


/*! \brief prints a string over uart followed by a newline (\n\r)
 *  \param string [in] string to print
 */
void uart_printstringln(char *string);

/*! \brief prints an int over uart followed by a newline (\n\r)
 *  \param num [in] int to print. Supports negative/positive ints.
 */
void uart_printintln(int num);

/*! \brief prints a float over uart followed by a newline (\n\r)
 *  \param string [in] float to print. Supports negative/positive floats.
 */
void uart_printfloatln (float n, int afterpoint);
