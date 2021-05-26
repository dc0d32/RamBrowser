/* Rambo : a simple RAM browser. Proof of concept for cold boot attack
 * Author : Prashant Borole (boroleprashant at mailing system from the Internet search giant)
 * */

#include <kb.h>
#include <screen.h>
#include <asm.h>

static const uint16_t normal_keytable[]={
	0, 27, '1', '2', '3', '4' ,'5', '6', '7',
	'8', '9', '0', '-', '=' ,'\b', '\t', 'q',
	'w', 'e', 'r', 't', 'y' ,'u', 'i', 'o',
	'p', '[', ']', /*ENTER*/'\n', 0 ,'a', 's', 'd',
	'f', 'g', 'h', 'j', 'k' ,'l', ';', '\'',
	'`', 0, '\\', 'z', 'x' ,'c', 'v', 'b',
	'n', 'm', ',', '.', '/' ,0, '*', 0,
	' ', 0, 0x3131, 0x3132, 0x3133 ,0x3134, 0x3135,0x3136,
	0x3137, 0x3138, 0x3139, 0x3130, 0 , 0, /*HOME*/0x3168, /*up*/0x3155,
	/*pgup*/0x313c, '-', /*left*/0x314c, 0x3135, /**right*/0x3152 ,'+', /*end*/0x3165, /*down*/0x3144,
	/*pgdn*/0x313e, /*ins*/0x3169, /*del*/0x3164, /*sysreq*/0x3153, 0, 0, /*f11*/0x315a ,/*f12*/0x3158, 0, 0,
	0x3171, 0x3177, /*MENU*/0x316d, 0,0,0,0,0
};

static const uint16_t shift_keytable[]={
	0, 27, '!', '@', '#', '$' ,'%', '^', '&',
	'*', '(', ')', '_', '+' ,'\b', '\t', 'Q',
	'W', 'E', 'R', 'T', 'Y' ,'U', 'I', 'O',
	'P', '{', '}', /*ENTER*/'\n', 0 ,'A', 'S', 'D',
	'F', 'G', 'H', 'J', 'K' ,'L', ':', '\"',
	'~', 0, '|', 'Z', 'X' ,'C', 'V', 'B',
	'N', 'M', '<', '>', '?' ,0, '*', 0,
	' ', 0, 0x3131, 0x3132, 0x3133 ,0x3134, 0x3135,0x3136,
	0x3137, 0x3138, 0x3139, 0x3130, 0 , 0, /*HOME*/0x3168, /*up*/0x3155,
	/*pgup*/0x313c, '-', /*left*/0x314c, 0x3135, /**right*/0x3152 ,'+', /*end*/0x3165, /*down*/0x3144,
	/*pgdn*/0x313e, /*ins*/0x3169, /*del*/0x3164, /*sysreq*/0x3153, 0, 0, /*f11*/0x315a ,/*f12*/0x3158, 0, 0,
	0x3171, 0x3177, /*MENU*/0x316d, 0,0,0,0,0
};


int8_t	kb_get_cooked(){
	static	bool	shift = false;
	uint8_t	c;
	while(true){
		c = wait_key();
		if(c == 0xE0){	// extended codes. not required for this demo
			wait_key();
			wait_key();
			continue;
		}
		if(c & 0x80){	// break code
			if(c == 0xaa || c == 0xb6){ // shift
				shift = false;
			}
			// any other break code is ignored ATM
		}
		else{	// make code
			if(c == 0x2a || c == 0x36){ // shift
				shift = true;
				continue;	// nothing to do further
			}
			if(shift)
				c = shift_keytable[c];
			else
				c = normal_keytable[c];
			if( (c >= 0x20 && c <= 0x7E ) || (c == '\n') || (c == '\b') )
				break;
		}
	}
	return c;
}

__NORETURN void kb_reboot()
{
	uint8_t temp;

	// flush the keyboard controller
	do
	{       
		temp = inb( 0x64 );
		if ( temp & 1 )
			inb( 0x60 );
	}
	while ( temp & 2 );

	// reboot
	outb(0xFE, 0x64);

	// spin if not restarted
	cls();
	puts("Cold reboot failed. Please manually reset your computer.");
	while(true);
}


uint8_t	wait_key(){
	while(!(inb(0x64) & 0x01));
	return inb(0x60);
}

size_t	kb_get_string(int8_t	*s, size_t	size){
	uint8_t	c;
	size_t	i = 0;
	while((c = kb_get_cooked()) != '\n' && i < size - 1){
		if((c == '\b')){
			if(i){
				putchar(c);
				--i;
			}
		}
		else{
			putchar(c);
			s[i++] = c;
		}
	}
	s[i] = '\0';
	putchar('\n');
	return i;
}

