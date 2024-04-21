#include <tcip_obsd_cfg.h>

#ifdef OBSD_PNIO_TOOL_CHAIN_MICROSOFT
#ifndef TOOL_CHAIN_MICROSOFT
#define TOOL_CHAIN_MICROSOFT 1
#endif
#elif defined OBSD_PNIO_TOOL_CHAIN_NONE
#ifndef TOOL_CHAIN_NONE
#define TOOL_CHAIN_NONE 1
#endif
#elif defined OBSD_PNIO_TOOL_CHAIN_TASKING_TRICORE
#ifndef TOOL_CHAIN_TASKING_TRICORE
#define TOOL_CHAIN_TASKING_TRICORE 1
#endif
#elif defined OBSD_PNIO_TOOL_CHAIN_GNU
#ifndef TOOL_CHAIN_GNU
#define TOOL_CHAIN_GNU 1
#endif
#elif defined OBSD_PNIO_TOOL_CHAIN_GNU_PPC
#ifndef TOOL_CHAIN_GNU_PPC
#define TOOL_CHAIN_GNU_PPC 1
#endif
#elif defined OBSD_PNIO_TOOL_CHAIN_CC386
#ifndef TOOL_CHAIN_CC386
#define TOOL_CHAIN_CC386 1
#endif
#elif defined OBSD_PNIO_TOOL_CHAIN_GREENHILLS_ARM
#ifndef TOOL_CHAIN_GREENHILLS_ARM
#define TOOL_CHAIN_GREENHILLS_ARM 1
#endif
#elif defined OBSD_PNIO_TOOL_CHAIN_NRK
#ifndef TOOL_CHAIN_NRK
#define TOOL_CHAIN_NRK 1
#endif
#else
#error Undefined compiler
#endif
