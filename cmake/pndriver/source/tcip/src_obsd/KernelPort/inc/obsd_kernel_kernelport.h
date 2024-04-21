/* ---------------------------------------------------------------------------------------------- */
/*                                                                                                */
/* project    : TCP-Stack for Profinet                                                            */
/* file       : obsd_kernel_kernelport.h                                                          */
/* description: include file with the interface of the kernelport layer                           */
/* compiler   : ANSI C                                                                            */
/* maschine   : miscellaneous                                                                     */
/*                                                                                                */
/* history:                                                                                       */
/* version  date      author            change                                                    */
/* ======= ========== ================  ========================================================= */
/*  0.01   30.10.2013 Gerlach, ATS 1    initial coding                                            */
/* ---------------------------------------------------------------------------------------------- */

#ifndef __OBSD_KERNELPORT_H__
#define __OBSD_KERNELPORT_H__


/* v===================== types ================================================================v */
/* ^============================================================================================^ */


/* v===================== function prototypes ==================================================v */

extern void StartStackKernel(void);
extern void StopStackKernel(void);

extern void Xintrnet(void*);        /* entrypoint for rx interrrupts (network driver)             */
extern void Xintrltimer(void);      /* enty point for clock interrupts, has to be called in a     */
                                    /* cycle of 10ms (typical OpenBSD clock), see param.c         */


/* ^============================================================================================^ */


#endif  /* __OBSD_KERNELPORT_H__ */
