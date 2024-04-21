#include <tcip_obsd_cfg.h> /* OBSD_ITGR -- endianess */

#if defined(_BYTE_ORDER)
	#error "check definition of _BYTE_ORDER"
#else
	#if (OBSD_PNIO_CFG_BIG_ENDIAN  == 1)
		#define _BYTE_ORDER _BIG_ENDIAN
	#elif (OBSD_PNIO_CFG_BIG_ENDIAN == 0)
		#define _BYTE_ORDER _LITTLE_ENDIAN
	#else
		#error "No endianess defined"
	#endif
#endif

