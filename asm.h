/* Rambo : a simple RAM browser. Proof of concept for cold boot attack
 * Author : Prashant Borole (boroleprashant at mailing system from the Internet search giant)
 * */
#ifndef _ASM_H_
#define _ASM_H_

#ifdef KERNEL

/*these macros must only be available in the assembly stub*/
#ifdef _STUB_S_
/*only asm specific portion*/

.extern init_kernel_stack
.extern multiboot_info_ptr
.extern multiboot_bootloader_magic

#else

#include <types.h>

#define IOMAKER(bwl, bw, type)                                          \
	static inline void out##bwl(type value, uint16_t port)          \
{                                                                       \
	__asm__ __volatile__("out" #bwl " %" #bw "0, %w1"       	\
			: : "a"(value), "d"(port));                     \
}									\
static inline type in##bwl(uint16_t port)                       	\
{              								\
	type val;                                                	\
	__asm__ __volatile__("in" #bwl " %w1, %" #bw "0"        	\
			:"=a"(val) : "d"(port));                        \
	return val;							\
}

IOMAKER(b, b, uint8_t)
IOMAKER(w, w, uint16_t)
IOMAKER(l, , uint32_t)

#endif /* ! _STUB_S_*/

#endif /* ! KERNEL */

#endif /* ! _ASM_H_*/

