#ifndef __OBSD_PLATFORM_OSAL_H__
#define __OBSD_PLATFORM_OSAL_H__

#include "obsd_platform_stack_config.h"


void                        OSAL_Init(void);
void                        OSAL_Deinit(void);
void                        OSAL_kernel_putchar(int c);
void                        OSAL_event_panic_reboot(int howto);
void                        OSAL_NotImplemented(const char * pchFuncName, const char * pchFileName);

/* flags for kern_malloc */
#define OSAL_M_ZERO 0x0008     /* must fit to M_ZERO Flag in OpenBSD */
void * OSAL_kern_malloc(unsigned long size, int type, int flags);
void   OSAL_kern_free(void *addr, int type);

/* dont'use size_t because we could have different size_t definitions in the OpenBSD Code and the */
/* platform specific part                                                                         */
void * OSAL_km_alloc(unsigned int Size);              
void   OSAL_km_free( void *pMem, unsigned int  Size);

#endif  /* __OBSD_PLATFORM_OSAL_H__ */
