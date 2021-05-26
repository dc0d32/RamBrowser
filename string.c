/* Rambo : a simple RAM browser. Proof of concept for cold boot attack
 * Author : Prashant Borole (boroleprashant at mailing system from the Internet search giant)
 * */

#include <string.h>

// max substring size : hardcoded as I intend to keep this demo small, which means
// no memory management support.
#define XSIZE 33

static __FASTCALL void kmp_precompute_table(int8_t *x, size_t m, int16_t *kmp_table) {
	kmp_table[0] = -1;
	size_t	i;
	for (i = 0; i < m; i++) {
		kmp_table[i + 1] = kmp_table[i] + 1;
		while (kmp_table[i + 1] > 0 &&
				x[i] != x[kmp_table[i + 1] - 1])
			kmp_table[i + 1] = kmp_table[kmp_table[i + 1] - 1] + 1;
	}
}


static __FASTCALL size_t kmp(int8_t *x, size_t m, int8_t *y, size_t n, int16_t *kmp_table) {

	/* Preprocessing */
	kmp_precompute_table(x, m, kmp_table);

	/* Searching */
	size_t	i, j;
	j = 0;
	for (i = 0; i < n - m + 1; i++)
		for (;;) {      // loop until break
			if (y[i] == x[j]) { // matches?
				j++;        // yes, move on to next state
				if (j == m) {   // maybe that was the last state
					return (i - j + 1);
/*					j = kmp_table[j]; // if we want to report more hits */
				}
				break;
			} else if (j == 0) break;   // no match in state j=0, give up
			else j = kmp_table[j];    // try shorter partial match
		}
	return n;
}

/* memfind : finds memory pointed to by q of size qs in a buffer of size ms pointed to by m. Uses Knuth-Morris-Pratt algorithm. */
__FASTCALL void	*memfind(void	*m, size_t	ms, void	*q, size_t	qs){
	int16_t	kmp_table[XSIZE];
	size_t	idx = kmp(q, qs, m, ms, kmp_table);
	if(idx == ms)
		return NULL;
	return m + idx;
}

#if 1

static __FASTCALL void kmp_precompute_table_r(int8_t *x, size_t m, int16_t *kmp_table) {
	kmp_table[0] = -1;
	size_t	i;
	for (i = 0; i < m; i++) {
		kmp_table[i + 1] = kmp_table[i] + 1;
		while (kmp_table[i + 1] > 0 &&
				x[m - 1 - i] != x[m - 1 - kmp_table[i + 1] - 1])
			kmp_table[i + 1] = kmp_table[kmp_table[i + 1] - 1] + 1;
	}
}


static __FASTCALL size_t kmp_r(int8_t *x, size_t m, int8_t *y, size_t n, int16_t *kmp_table) {

	/* Preprocessing */
	kmp_precompute_table_r(x, m, kmp_table);

	/* Searching */
	size_t	i, j;
	j = 0;
	for (i = 0; i < n - m + 1; i++)
		for (;;) {      // loop until break
			if (y[n - 1 - i] == x[m - 1 - j]) { // matches?
				j++;        // yes, move on to next state
				if (j == m) {   // maybe that was the last state
					return ((n - 1 - i) - (m - 1 - j) - 1);
/*					j = kmp_table[j]; // if we want to report more hits */
				}
				break;
			} else if (j == 0) break;   // no match in state j=0, give up
			else j = kmp_table[j];    // try shorter partial match
		}
	return n;
}

/* memfind : finds memory pointed to by q of size qs in a buffer of size ms pointed to by m, in reverse order ie. starting from end of m and scanning towards begining of m. Uses Knuth-Morris-Pratt algorithm. */
__FASTCALL void	*memfind_r(void	*m, size_t	ms, void	*q, size_t	qs){
	int16_t	kmp_table[XSIZE];
	size_t	idx = kmp_r(q, qs, m, ms, kmp_table);
	if(idx == ms)
		return NULL;
	return m + idx;
}
#endif

