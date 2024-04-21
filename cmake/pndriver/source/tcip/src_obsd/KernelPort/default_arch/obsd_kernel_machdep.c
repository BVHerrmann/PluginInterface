
/* gh2289n: ##automatically insert project defines here## (done) */
/* gerlach/g2289n: need to add #define _KERNEL for all Kernel files because PN Integration has no */
/*                 possibilities to define _KERNEL in Kernel only project files (PN has only      */
/*                 one project for Kernel and userland sources - a restriction of the PN          */
/*                 development environment). So we have to mark all Kernel C-files manually by    */
/*                 adding _KERNEL in all the files itself.                                        */
#define _KERNEL
#define INET
/* gh2289n: ##automatically insert project defines here## (end) */

#include <sys/obsd_kernel_param.h>
#include <sys/obsd_kernel_systm.h>

#include <machine/obsd_kernel_cpu.h>

/*
 * Statically-allocated CPU info for the primary CPU (or the only
 * CPU, on uniprocessors).  The CPU info list is initialized to
 * point at it.
 */

#ifndef MULTIPROCESSOR
struct cpu_info cpu_info_primary;
#endif /* MULTIPROCESSOR */


struct cpu_info *cpu_info_list = &cpu_info_primary;

void init386(void)
{
    memset((void*)&cpu_info_primary, 0, sizeof(cpu_info_primary));
    /*cpu_info_primary.ci_self = &cpu_info_primary; */
}

void deinit386(void)
{
}

void cpu_deinitclocks(void)
{
    hw_deinitclocks();
}

/* gerlach/gh2289n */
/* normally in sys\arch\i386\include\cpu.h */
/* normally curcpu is a macro like 
 *  #define	curcpu()		(&cpu_info_primary)   as defined in cpu.h 
 * And normally cpu_info_primary.ci_curproc is set to the current process within cpu_switchto (locore.s)
 * when switching the processes. But because our processes / threads are switches by an external scheduler
 * we have to synchronize our process list and cpu_info_primary.ci_curproc (which is used via the curproc
 * macro in sys\systm.h as curcpu()->ci_curproc many times in the stack). One way to do this is the curcpu call
 */

struct cpu_info *curcpu(void)
{
    /*cpu_info_primary.ci_curproc = proc_get_curproc();*/
    return &cpu_info_primary;
}



void cpu_initclocks(void)
{
    hw_initclocks();
}
