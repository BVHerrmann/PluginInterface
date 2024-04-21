/*	$OpenBSD: errno.c,v 1.5 2005/08/08 08:05:34 espie Exp $ */
/* PUBLIC DOMAIN: No Rights Reserved.   Marco S Hyman <marc@snafu.org> */

#include <obsd_userland_errno.h>
#undef errno

#include <sys/obsd_kernel_param.h>
#include <sys/obsd_kernel_systm.h>
#include <sys/obsd_kernel_proc.h>

static int global_errno = ESRCH;

int *obsd__errno(void)
{
    upls_t * pupls = obsd_get_upls();  /* may return NULL */
    if (pupls != NULL)
    {
        return  &(pupls->pls_errno);
    }
    else
    {   /* process unknown and can't be registered */
        global_errno = ESRCH;   /* in cases someone overwrote it */
        return &global_errno;
    }
}
