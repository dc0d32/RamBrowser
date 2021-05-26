/* Rambo : a simple RAM browser. Proof of concept for cold boot attack
 * Author : Prashant Borole (boroleprashant at mailing system from the Internet search giant)
 * */

#ifndef _STRING_H_
#define _STRING_H_

#include<types.h>

__FASTCALL __INLINE inline uint32_t strlen(int8_t *s){
	register uint32_t len = 0xFFFFFFFFU;
	__asm__ (
			"cld\n\trepne"
			"\n\tscasb"
			:"=c"(len), "=D"(s)
			:"D"(s), "c"(len), "a"(0)
			:"memory", "cc"
			);
	return (0xFFFFFFFFU - len - 1);
}



__FASTCALL __INLINE inline void* memcpy(register void *d, register void* s, register uint32_t src_len){

	register void * dest = d;

	__asm__ ("cld");

	if(s < d && ((int8_t *)s + src_len >= (int8_t *)d)){	//copy backwards
		__asm__ __volatile__ ("std");
		d = (int8_t *)d + src_len - 1;
		s = (int8_t *)s + src_len - 1;
	}

	register uint32_t trailing_len = src_len % 4;
	src_len /= 4;

	__asm__ (
			"rep"
			"\n\tmovsl"
			:"=S"(s), "=D"(d), "=c"(src_len)
			:"S"(s), "D"(d), "c"(src_len)
			);

	__asm__ (
			"rep"
			"\n\tmovsb"
			:"=S"(s), "=D"(d), "=c"(src_len)
			:"S"(s), "D"(d), "c"(trailing_len)
			);

	return (dest);
}

#if 0
__FASTCALL __INLINE inline void* memset(void *ptr, int8_t c, uint32_t count){
	register uint32_t trailing_len = count%4;
	register void* temp = ptr;

	__asm__ (
			"cld\n\trep"
			"\n\tstosl"
			:"=D"(ptr), "=c"(count), "=a"(c)
			:"D"(ptr), "c"(count/4) /*copy 4 bytes at a time {REALLY FAST}*/,
			"a"(c<<24|c<<16|c<<8|c)
			:"memory", "cc"
			);
	__asm__ (
			"rep"
			"\n\tstosb"
			:"=D"(ptr), "=c"(count), "=a"(c)
			:"D"(ptr), "c"(trailing_len), "a"(c)
			:"memory", "cc"
			);
	return (temp);
}
#endif

__FASTCALL __INLINE inline void* memsetw(void *ptr, int16_t c, uint32_t count){
	register uint32_t trailing_len = count%2;
	register void* temp = ptr;

	__asm__ (
			"cld\n\trep"
			"\n\tstosl"
			:"=D"(ptr), "=c"(count), "=a"(c)
			:"D"(ptr), "c"(count/2), //copy 4 bytes at a time {REALLY FAST}
			"a"(c<<16|c)
			:"memory", "cc"
			);
	if(trailing_len){
		*(int16_t*)ptr = c;
	}
	return (temp);
}


__FASTCALL __INLINE inline int32_t memcmp(void *_d, void	*_s, size_t src_len){
	size_t	l = src_len;
	uint8_t	*s = _s;
	uint8_t	*d = _d;
	while(*s++ == *d++ && l--);
	return l;
}


/* memfind : finds memory pointed to by q of size qs in a buffer of size ms pointed to by m. Uses Knuth-Morris-Pratt algorithm. */
__FASTCALL void	*memfind(void	*m, size_t	ms, void	*q, size_t	qs);

/* memfind : finds memory pointed to by q of size qs in a buffer of size ms pointed to by m, in reverse order ie. starting from end of m and scanning towards begining of m. Uses Knuth-Morris-Pratt algorithm. */
__FASTCALL void	*memfind_r(void	*m, size_t	ms, void	*q, size_t	qs);

#endif

