#ifndef __TCIP_OBSD_TRC_H__
#define __TCIP_OBSD_TRC_H__

#include <sys/obsd_kernel_types.h>

enum obsd_level {
  OBSD_TRACE_LEVEL_OFF,
  OBSD_TRACE_LEVEL_FATAL,
  OBSD_TRACE_LEVEL_ERROR,
  OBSD_TRACE_LEVEL_UNEXP,
  OBSD_TRACE_LEVEL_WARN,
  OBSD_TRACE_LEVEL_NOTE_HIGH,
  OBSD_TRACE_LEVEL_NOTE,
  OBSD_TRACE_LEVEL_NOTE_LOW,
  OBSD_TRACE_LEVEL_CHAT
};

#define OBSD_LEVEL_TYPE enum obsd_level

#ifdef PNTEST_USE_TCIP_CFG_OBSD

#define TCIP_OBSD_TRACE_00(idx, level, msg)
#define TCIP_OBSD_TRACE_01(idx, level, msg, para1)
#define TCIP_OBSD_TRACE_02(idx, level, msg, para1, para2)
#define TCIP_OBSD_TRACE_03(idx, level, msg, para1, para2, para3)
#define TCIP_OBSD_TRACE_04(idx, level, msg, para1, para2, para3, para4)
#define TCIP_OBSD_TRACE_05(idx, level, msg, para1, para2, para3, para4, para5)
#define TCIP_OBSD_TRACE_06(idx, level, msg, para1, para2, para3, para4, para5, para6)
#define TCIP_OBSD_TRACE_07(idx, level, msg, para1, para2, para3, para4, para5, para6, para7)
#define TCIP_OBSD_TRACE_08(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8)
#define TCIP_OBSD_TRACE_09(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9)
#define TCIP_OBSD_TRACE_10(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10)
#define TCIP_OBSD_TRACE_11(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11)
#define TCIP_OBSD_TRACE_12(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12)
#define TCIP_OBSD_TRACE_13(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12, para13)
#define TCIP_OBSD_TRACE_14(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12, para13, para14)
#define TCIP_OBSD_TRACE_15(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12, para13, para14, para15)
#define TCIP_OBSD_TRACE_16(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12, para13, para14, para15, para16)

#elif OBSD_PNIO_CFG_TRACE_MODE == 0

#define TCIP_OBSD_TRACE_00(idx, level, msg)
#define TCIP_OBSD_TRACE_01(idx, level, msg, para1)
#define TCIP_OBSD_TRACE_02(idx, level, msg, para1, para2)
#define TCIP_OBSD_TRACE_03(idx, level, msg, para1, para2, para3)
#define TCIP_OBSD_TRACE_04(idx, level, msg, para1, para2, para3, para4)
#define TCIP_OBSD_TRACE_05(idx, level, msg, para1, para2, para3, para4, para5)
#define TCIP_OBSD_TRACE_06(idx, level, msg, para1, para2, para3, para4, para5, para6)
#define TCIP_OBSD_TRACE_07(idx, level, msg, para1, para2, para3, para4, para5, para6, para7)
#define TCIP_OBSD_TRACE_08(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8)
#define TCIP_OBSD_TRACE_09(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9)
#define TCIP_OBSD_TRACE_10(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10)
#define TCIP_OBSD_TRACE_11(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11)
#define TCIP_OBSD_TRACE_12(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12)
#define TCIP_OBSD_TRACE_13(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12, para13)
#define TCIP_OBSD_TRACE_14(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12, para13, para14)
#define TCIP_OBSD_TRACE_15(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12, para13, para14, para15)
#define TCIP_OBSD_TRACE_16(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12, para13, para14, para15, para16)

#elif OBSD_PNIO_CFG_TRACE_MODE == 1

#define TCIP_OBSD_TRACE_00(idx, level, msg)
#define TCIP_OBSD_TRACE_01(idx, level, msg, para1)
#define TCIP_OBSD_TRACE_02(idx, level, msg, para1, para2)
#define TCIP_OBSD_TRACE_03(idx, level, msg, para1, para2, para3)
#define TCIP_OBSD_TRACE_04(idx, level, msg, para1, para2, para3, para4)
#define TCIP_OBSD_TRACE_05(idx, level, msg, para1, para2, para3, para4, para5)
#define TCIP_OBSD_TRACE_06(idx, level, msg, para1, para2, para3, para4, para5, para6)
#define TCIP_OBSD_TRACE_07(idx, level, msg, para1, para2, para3, para4, para5, para6, para7)
#define TCIP_OBSD_TRACE_08(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8)
#define TCIP_OBSD_TRACE_09(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9)
#define TCIP_OBSD_TRACE_10(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10)
#define TCIP_OBSD_TRACE_11(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11)
#define TCIP_OBSD_TRACE_12(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12)
#define TCIP_OBSD_TRACE_13(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12, para13)
#define TCIP_OBSD_TRACE_14(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12, para13, para14)
#define TCIP_OBSD_TRACE_15(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12, para13, para14, para15)
#define TCIP_OBSD_TRACE_16(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12, para13, para14, para15, para16)

#elif OBSD_PNIO_CFG_TRACE_MODE == 2

void obsd_pn_trace_00(uint32_t, uint32_t, uint32_t, OBSD_LEVEL_TYPE, char*);
void obsd_pn_trace_01(uint32_t, uint32_t, uint32_t, OBSD_LEVEL_TYPE, char*, uint32_t);
void obsd_pn_trace_02(uint32_t, uint32_t, uint32_t, OBSD_LEVEL_TYPE, char*, uint32_t, uint32_t);
void obsd_pn_trace_03(uint32_t, uint32_t, uint32_t, OBSD_LEVEL_TYPE, char*, uint32_t, uint32_t, uint32_t);
void obsd_pn_trace_04(uint32_t, uint32_t, uint32_t, OBSD_LEVEL_TYPE, char*, uint32_t, uint32_t, uint32_t, uint32_t);
void obsd_pn_trace_05(uint32_t, uint32_t, uint32_t, OBSD_LEVEL_TYPE, char*, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
void obsd_pn_trace_06(uint32_t, uint32_t, uint32_t, OBSD_LEVEL_TYPE, char*, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
void obsd_pn_trace_07(uint32_t, uint32_t, uint32_t, OBSD_LEVEL_TYPE, char*, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
void obsd_pn_trace_08(uint32_t, uint32_t, uint32_t, OBSD_LEVEL_TYPE, char*, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
void obsd_pn_trace_09(uint32_t, uint32_t, uint32_t, OBSD_LEVEL_TYPE, char*, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
void obsd_pn_trace_10(uint32_t, uint32_t, uint32_t, OBSD_LEVEL_TYPE, char*, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
void obsd_pn_trace_11(uint32_t, uint32_t, uint32_t, OBSD_LEVEL_TYPE, char*, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
void obsd_pn_trace_12(uint32_t, uint32_t, uint32_t, OBSD_LEVEL_TYPE, char*, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
void obsd_pn_trace_13(uint32_t, uint32_t, uint32_t, OBSD_LEVEL_TYPE, char*, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
void obsd_pn_trace_14(uint32_t, uint32_t, uint32_t, OBSD_LEVEL_TYPE, char*, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
void obsd_pn_trace_15(uint32_t, uint32_t, uint32_t, OBSD_LEVEL_TYPE, char*, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
void obsd_pn_trace_16(uint32_t, uint32_t, uint32_t, OBSD_LEVEL_TYPE, char*, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);

#define TCIP_OBSD_TRACE_00(idx, level, msg)                                                                                                                          obsd_pn_trace_00(idx, LTRC_ACT_MODUL_ID, __LINE__, level, msg);
#define TCIP_OBSD_TRACE_01(idx, level, msg, para1)                                                                                                                   obsd_pn_trace_01(idx, LTRC_ACT_MODUL_ID, __LINE__, level, msg, para1);
#define TCIP_OBSD_TRACE_02(idx, level, msg, para1, para2)                                                                                                            obsd_pn_trace_02(idx, LTRC_ACT_MODUL_ID, __LINE__, level, msg, para1, para2);
#define TCIP_OBSD_TRACE_03(idx, level, msg, para1, para2, para3)                                                                                                     obsd_pn_trace_03(idx, LTRC_ACT_MODUL_ID, __LINE__, level, msg, para1, para2, para3);
#define TCIP_OBSD_TRACE_04(idx, level, msg, para1, para2, para3, para4)                                                                                              obsd_pn_trace_04(idx, LTRC_ACT_MODUL_ID, __LINE__, level, msg, para1, para2, para3, para4);
#define TCIP_OBSD_TRACE_05(idx, level, msg, para1, para2, para3, para4, para5)                                                                                       obsd_pn_trace_05(idx, LTRC_ACT_MODUL_ID, __LINE__, level, msg, para1, para2, para3, para4, para5);
#define TCIP_OBSD_TRACE_06(idx, level, msg, para1, para2, para3, para4, para5, para6)                                                                                obsd_pn_trace_06(idx, LTRC_ACT_MODUL_ID, __LINE__, level, msg, para1, para2, para3, para4, para5, para6);
#define TCIP_OBSD_TRACE_07(idx, level, msg, para1, para2, para3, para4, para5, para6, para7)                                                                         obsd_pn_trace_07(idx, LTRC_ACT_MODUL_ID, __LINE__, level, msg, para1, para2, para3, para4, para5, para6, para7);
#define TCIP_OBSD_TRACE_08(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8)                                                                  obsd_pn_trace_08(idx, LTRC_ACT_MODUL_ID, __LINE__, level, msg, para1, para2, para3, para4, para5, para6, para7, para8);
#define TCIP_OBSD_TRACE_09(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9)                                                           obsd_pn_trace_09(idx, LTRC_ACT_MODUL_ID, __LINE__, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9);
#define TCIP_OBSD_TRACE_10(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10)                                                   obsd_pn_trace_10(idx, LTRC_ACT_MODUL_ID, __LINE__, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10);
#define TCIP_OBSD_TRACE_11(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11)                                           obsd_pn_trace_11(idx, LTRC_ACT_MODUL_ID, __LINE__, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11);
#define TCIP_OBSD_TRACE_12(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12)                                   obsd_pn_trace_12(idx, LTRC_ACT_MODUL_ID, __LINE__, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12);
#define TCIP_OBSD_TRACE_13(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12, para13)                           obsd_pn_trace_13(idx, LTRC_ACT_MODUL_ID, __LINE__, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12, para13);
#define TCIP_OBSD_TRACE_14(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12, para13, para14)                   obsd_pn_trace_14(idx, LTRC_ACT_MODUL_ID, __LINE__, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12, para13, para14);
#define TCIP_OBSD_TRACE_15(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12, para13, para14, para15)           obsd_pn_trace_15(idx, LTRC_ACT_MODUL_ID, __LINE__, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12, para13, para14, para15);
#define TCIP_OBSD_TRACE_16(idx, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12, para13, para14, para15, para16)   obsd_pn_trace_16(idx, LTRC_ACT_MODUL_ID, __LINE__, level, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12, para13, para14, para15, para16);

#else

#error "OBSD_PNIO_CFG_TRACE_MODE has unknown value"

#endif

#endif /* __TCIP_OBSD_TRC_H__ */
