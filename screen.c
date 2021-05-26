/* Rambo : a simple RAM browser. Proof of concept for cold boot attack
 * Author : Prashant Borole (boroleprashant at mailing system from the Internet search giant)
 * */

#include <screen.h>
#include <string.h>

#include <asm.h>

/* Save the X position.  */
static uint32_t xpos = 0;
/* Save the Y position.  */
static uint32_t ypos = 0;
/* Points to the video memory.  */
static volatile uint8_t *video = (uint8_t*)VIDEO;


static void update_caret(){
	uint16_t	position=(ypos*80) + xpos;

	// cursor LOW port to vga INDEX register
	outb(0x0F, 0x3D4);
	outb((uint8_t)(position&0xFF), 0x3D5);
	// cursor HIGH port to vga INDEX register
	outb(0x0E, 0x3D4);
	outb((uint8_t)((position>>8)&0xFF), 0x3D5);
}



/* Clear the screen and initialize VIDEO, XPOS and YPOS.  */
	void
cls (void)
{
	uint32_t i;

	video = (uint8_t *) VIDEO;

	for (i = 0; i < COLUMNS * LINES * 2; i++)
		*(video + i) = 0;



	xpos = 0;
	ypos = 0;
	update_caret();
}



/* Convert the integer D to a string and save the string in BUF. If
   BASE is equal to 'd', intererpret that D is decimal, and if BASE is
   equal to 'x', interpret that D is hexadecimal.  */
	void __FASTCALL
itoa (int8_t *buf, int32_t base, int32_t d)
{
	int8_t *p = buf;
	int8_t *p1, *p2;
	uint32_t ud = d;
	int32_t divisor = 10;

	/* If %d is specified and D is minus, put `-' in the head.  */
	if (base == 'd' && d < 0)
	{
		*p++ = '-';
		buf++;
		ud = -d;
	}
	else if (base == 'x')
		divisor = 16;

	/* Divide UD by DIVISOR until UD == 0.  */
	do
	{
		int32_t remainder = ud % divisor;

		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
	}
	while (ud /= divisor);

	/* Terminate BUF.  */
	*p = 0;

	/* Reverse BUF.  */
	p1 = buf;
	p2 = p - 1;
	while (p1 < p2)
	{
		int8_t tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}
}

/* hatoi : hex string -> unsigned 32bit integer*/
	uint32_t __FASTCALL
hatoi (int8_t *p)
{
	uint32_t num = 0;
	while(*p){
		uint32_t	i = 0;
		if(*p <= '9')
			i = *p - '0';
		else if(*p <= 'F')
			i = 10 + *p - 'A';
		else if(*p <= 'f')
			i = 10 + *p - 'a';
		num = (num << 4) | (i & 0x0F);
		p++;
	}
	return num;
}

/* Put the character C on the screen. */
	void __FASTCALL
putchar (uint8_t c)
{
	if (c == '\n' )
	{
		xpos = 0;
		if (ypos >= LINES){
			ypos=0;
		}
		else{
			ypos++;
		}

		memsetw((int8_t*)(video + ypos * COLUMNS * 2), (ATTRIBUTE << 8), 80);
		update_caret();
		return;
	}
	else if(c == '\b'){
		if(xpos){
			--xpos;
			putchar(' ');
			--xpos;
			update_caret();
		}
		return;
	}
	else if(c == '\r'){
		xpos = 0;
		update_caret();
		return;
	}

	*(video + (xpos + ypos * COLUMNS) * 2) = c ;
	*(video + (xpos + ypos * COLUMNS) * 2 + 1) = ATTRIBUTE;

	xpos++;
	if (xpos >= COLUMNS)
		putchar('\n');

	update_caret();
}


void puts(int8_t	*s){
	while (*s)
		putchar (*s++);
}

/* Format a string and print it on the screen, just like the libc
   function printf.  */
	void
printf (const int8_t *format, ...)
{
	int32_t *arg = (int32_t*)&format;
	int32_t c;
	int8_t buf[20];

	arg++;

	while ((c = *format++) != 0)
	{
		if (c != '%')
			putchar (c);
		else
		{
			int8_t *p;

			c = *format++;
			switch (c)
			{
				case 'd':
				case 'u':
				case 'x':
				case 'h':
					itoa (buf, c, *arg++);
					p = buf;
					puts(p);
					break;


				case 's':
					p = (int8_t*)(*arg++);
					if (! p)
						p = "(null)";
					puts(p);
					break;

				default:
					puts("(invalid conversion specifier)");
					break;
			}
		}
	}
}

