/* Rambo : a simple RAM browser. Proof of concept for cold boot attack
 * Author : Prashant Borole (boroleprashant at mailing system from the Internet search giant)
 * */


#ifndef _TYPES_H_
#define _TYPES_H_

#define __FASTCALL __attribute__(( regparm(3) ))
#define __NORETURN __attribute (( noreturn ))
#define __INLINE __attribute__ ((always_inline))

/*tell compiler NOT to align the entries of the struct to word/dword for performance*/
#define __PACKED __attribute__((packed))

typedef unsigned char	uint8_t;
typedef char	int8_t;

typedef unsigned short	uint16_t;
typedef signed short	int16_t;

typedef unsigned int	uint32_t;
typedef signed int	int32_t;

typedef unsigned int	size_t;

#ifndef __cplusplus
	typedef enum {false, true}	bool;
#endif

/*needs no explanation*/
#define NULL (void *)0

#endif /* ! _TYPES_H_*/
