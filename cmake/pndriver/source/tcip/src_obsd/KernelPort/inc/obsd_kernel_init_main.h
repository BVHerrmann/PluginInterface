/* ---------------------------------------------------------------------------------------------- */
/*                                                                                                */
/* project    : TCP-Stack for Profinet                                                            */
/* file       : obsd_kernel_init_main.h                                                           */
/* description: include file with the interface of the init_main module                           */
/* compiler   : ANSI C                                                                            */
/* maschine   : miscellaneous                                                                     */
/*                                                                                                */
/* history:                                                                                       */
/* version  date      author            change                                                    */
/* ======= ========== ================  ========================================================= */
/*  0.01   20.11.2013 Gerlach, ATS 1    initial coding                                            */
/* ---------------------------------------------------------------------------------------------- */

#ifndef __OBSD_INIT_MAIN_H__
#define __OBSD_INIT_MAIN_H__


/* v===================== types ================================================================v */
/* ^============================================================================================^ */


/* v===================== function prototypes ==================================================v */

extern void Kernel_init(void);
extern void Kernel_deinit(void);
extern int  get_kernel_is_initialized(void);

/* ^============================================================================================^ */


#endif  /* __OBSD_INIT_MAIN_H__ */
