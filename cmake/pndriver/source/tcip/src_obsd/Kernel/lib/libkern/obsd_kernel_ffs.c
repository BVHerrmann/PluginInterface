/*	$OpenBSD: ffs.c,v 1.8 2009/10/27 23:59:35 deraadt Exp $	*/

/*
 * Public domain.
 * Written by Dale Rahn.
 */

/* gh2289n: ##automatically insert project defines here## (done) */
/* gerlach/g2289n: need to add #define _KERNEL for all Kernel files because PN Integration has no */
/*                 possibilities to define _KERNEL in Kernel only project files (PN has only      */
/*                 one project for Kernel and userland sources - a restriction of the PN          */
/*                 development environment). So we have to mark all Kernel C-files manually by    */
/*                 adding _KERNEL in all the files itself.                                        */
#define _KERNEL
#define INET
/* gh2289n: ##automatically insert project defines here## (end) */

#if !defined(_KERNEL) && !defined(_STANDALONE)
#include <obsd_kernel_string.h>
#else
#include <lib/libkern/obsd_kernel_libkern.h>
#endif

/*
 * ffs -- vax ffs instruction
 */
int
ffs(int mask)
{
	int bit;
	unsigned int r = mask;
	static const signed char t[16] = {
		-28, 1, 2, 1,
		  3, 1, 2, 1,
		  4, 1, 2, 1,
		  3, 1, 2, 1
	};

	bit = 0;
	if (!(r & 0xffff)) {
		bit += 16;
		r >>= 16;
	}
	if (!(r & 0xff)) {
		bit += 8;
		r >>= 8;
	}
	if (!(r & 0xf)) {
		bit += 4;
		r >>= 4;
	}

	return (bit + t[ r & 0xf ]);
}
