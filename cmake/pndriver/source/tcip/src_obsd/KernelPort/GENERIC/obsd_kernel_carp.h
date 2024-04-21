#ifndef OBSD_CARP
#include "obsd_kernel_BSDStackPort.h"   /* gh2289n: though all files should include this anyway we have (in some file) a */
                                        /*          problem with the order: some files include carp.h before params.h or */
                                        /*          other includes that BSDStackPort.h. But we need OBSD_CARP here.      */
                                        /*          So explicitly include BSDStackPort.h (to get                         */
                                        /*          obsd_platform_stack_config.h) here if necessary.                     */
#endif /* OBSD_CARP */

/* use setting defined in obsd_platform_stack_config.h */
#define NCARP   OBSD_CARP
