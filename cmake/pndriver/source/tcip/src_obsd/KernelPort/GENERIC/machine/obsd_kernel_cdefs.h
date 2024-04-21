/*	$OpenBSD: cdefs.h,v 1.9 2005/11/24 20:46:45 deraadt Exp $	*/

/*
 * Written by J.T. Conklin <jtc@wimsey.com> 01/17/95.
 * Public domain.
 */

#ifndef	_MACHINE_CDEFS_H_
#define	_MACHINE_CDEFS_H_

#include "obsd_kernel_BSDStackPort.h"  /* gh2289n: can't use condition BSD_STACKPORT, because it's defined in the header itself */

#define __weak_alias(alias,sym)  

#endif /* !_MACHINE_CDEFS_H_ */
