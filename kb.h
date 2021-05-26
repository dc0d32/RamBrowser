/* Rambo : a simple RAM browser. Proof of concept for cold boot attack
 * Author : Prashant Borole (boroleprashant at mailing system from the Internet search giant)
 * */

#ifndef _KB_H_
#define _KB_H_

#include <types.h>

extern uint8_t wait_key();
extern __NORETURN void kb_reboot();

int8_t	kb_get_cooked();
size_t	kb_get_string(int8_t	*s, size_t	size);

#endif /* _KB_H_ */
