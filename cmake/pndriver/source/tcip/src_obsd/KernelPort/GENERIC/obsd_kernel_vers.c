/* gh2289n: ##automatically insert project defines here## (done) */
/* gerlach/g2289n: need to add #define _KERNEL for all Kernel files because PN Integration has no */
/*                 possibilities to define _KERNEL in Kernel only project files (PN has only      */
/*                 one project for Kernel and userland sources - a restriction of the PN          */
/*                 development environment). So we have to mark all Kernel C-files manually by    */
/*                 adding _KERNEL in all the files itself.                                        */
#define _KERNEL
#define INET
/* gh2289n: ##automatically insert project defines here## (end) */

#define STATUS ""			/* release */
#if 0
#define STATUS "-beta"			/* just before a release */
#define STATUS "-current"		/* just after a release */
#endif

const char ostype[] = "OpenBSD";
const char osrelease[] = "5.1";
const char osversion[] = "GENERIC#0";
const char sccs[] =
    "    @(#)OpenBSD 5.1" STATUS " (GENERIC) #0: Tue May  8 20:33:56 CEST 2012\n";
const char version[] =
    "OpenBSD 5.1" STATUS " (GENERIC) #0: Tue May  8 20:33:56 CEST 2012\n    root@OBSD51.my.domain:/usr/src/sys/arch/i386/compile/GENERIC\n";
