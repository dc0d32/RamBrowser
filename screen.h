/* Rambo : a simple RAM browser. Proof of concept for cold boot attack
 * Author : Prashant Borole (boroleprashant at mailing system from the Internet search giant)
 * */

#ifndef _SCREEN_H_
#define _SCREEN_H_

#include <types.h>

/* Some screen stuff.  */
/* The number of columns.  */
#define COLUMNS			80
/* The number of lines.  */
#define LINES			25
/* The attribute of an character.  */
#define ATTRIBUTE		0x07
/* The video memory address.  */
#define VIDEO			(0xB8000)

/* Clear the screen and initialize VIDEO, XPOS and YPOS.  */
void cls (void);

/* Convert the integer D to a string and save the string in BUF. If
   BASE is equal to 'd', intererpret that D is decimal, and if BASE is
   equal to 'x', interpret that D is hexadecimal.  */
void __FASTCALL itoa (int8_t *buf, int32_t base, int32_t d);

/* hatoi : hex string -> unsigned 32bit integer*/
uint32_t __FASTCALL hatoi (int8_t *p);

/* Put the character C on the screen. */
void __FASTCALL putchar (uint8_t c);

/* print a string */
void puts(int8_t	*s);

/* Format a string and print it on the screen, just like the libc
   function printf.  */
void printf (const int8_t *format, ...);

#endif /* _SCREEN_H_ */
