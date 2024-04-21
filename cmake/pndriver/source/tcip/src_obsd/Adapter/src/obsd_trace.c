#define LTRC_ACT_MODUL_ID 4000

#include <tcip_inc.h>
#include <tcip_trc.h>
#include <tcip_obsd_trc.h>



void obsd_pn_trace_00(uint32_t Idx, uint32_t ModuleID, uint32_t Line, OBSD_LEVEL_TYPE TraceLevel, char* msg)
{
    TCIP_OBSD_TRACE_00_EXT(Idx, (LTRC_LEVEL_TYPE)TraceLevel, ModuleID, Line, msg);
}

void obsd_pn_trace_01(uint32_t Idx, uint32_t ModuleID, uint32_t Line, OBSD_LEVEL_TYPE TraceLevel, char* msg, uint32_t para1)
{
    TCIP_OBSD_TRACE_01_EXT(Idx, (LTRC_LEVEL_TYPE)TraceLevel, ModuleID, Line, msg, para1);
}

void obsd_pn_trace_02(uint32_t Idx, uint32_t ModuleID, uint32_t Line, OBSD_LEVEL_TYPE TraceLevel, char* msg, uint32_t para1, uint32_t para2)
{
    TCIP_OBSD_TRACE_02_EXT(Idx, (LTRC_LEVEL_TYPE)TraceLevel, ModuleID, Line, msg, para1, para2);
}

void obsd_pn_trace_03(uint32_t Idx, uint32_t ModuleID, uint32_t Line, OBSD_LEVEL_TYPE TraceLevel, char* msg, uint32_t para1, uint32_t para2, uint32_t para3)
{
    TCIP_OBSD_TRACE_03_EXT(Idx, (LTRC_LEVEL_TYPE)TraceLevel, ModuleID, Line, msg, para1, para2, para3);
}

void obsd_pn_trace_04(uint32_t Idx, uint32_t ModuleID, uint32_t Line, OBSD_LEVEL_TYPE TraceLevel, char* msg, uint32_t para1, uint32_t para2, uint32_t para3, uint32_t para4)
{
    TCIP_OBSD_TRACE_04_EXT(Idx, (LTRC_LEVEL_TYPE)TraceLevel, ModuleID, Line, msg, para1, para2, para3, para4);
}

void obsd_pn_trace_05(uint32_t Idx, uint32_t ModuleID, uint32_t Line, OBSD_LEVEL_TYPE TraceLevel, char* msg, uint32_t para1, uint32_t para2, uint32_t para3, uint32_t para4, uint32_t para5)
{
    TCIP_OBSD_TRACE_05_EXT(Idx, (LTRC_LEVEL_TYPE)TraceLevel, ModuleID, Line, msg, para1, para2, para3, para4, para5);
}

void obsd_pn_trace_06(uint32_t Idx, uint32_t ModuleID, uint32_t Line, OBSD_LEVEL_TYPE TraceLevel, char* msg, uint32_t para1, uint32_t para2, uint32_t para3, uint32_t para4, uint32_t para5, uint32_t para6)
{
    TCIP_OBSD_TRACE_06_EXT(Idx, (LTRC_LEVEL_TYPE)TraceLevel, ModuleID, Line, msg, para1, para2, para3, para4, para5, para6);
}

void obsd_pn_trace_07(uint32_t Idx, uint32_t ModuleID, uint32_t Line, OBSD_LEVEL_TYPE TraceLevel, char* msg, uint32_t para1, uint32_t para2, uint32_t para3, uint32_t para4, uint32_t para5, uint32_t para6, uint32_t para7)
{
    TCIP_OBSD_TRACE_07_EXT(Idx, (LTRC_LEVEL_TYPE)TraceLevel, ModuleID, Line, msg, para1, para2, para3, para4, para5, para6, para7);
}

void obsd_pn_trace_08(uint32_t Idx, uint32_t ModuleID, uint32_t Line, OBSD_LEVEL_TYPE TraceLevel, char* msg, uint32_t para1, uint32_t para2, uint32_t para3, uint32_t para4, uint32_t para5, uint32_t para6, uint32_t para7, uint32_t para8)
{
    TCIP_OBSD_TRACE_08_EXT(Idx, (LTRC_LEVEL_TYPE)TraceLevel, ModuleID, Line, msg, para1, para2, para3, para4, para5, para6, para7, para8);
}

void obsd_pn_trace_09(uint32_t Idx, uint32_t ModuleID, uint32_t Line, OBSD_LEVEL_TYPE TraceLevel, char* msg, uint32_t para1, uint32_t para2, uint32_t para3, uint32_t para4, uint32_t para5, uint32_t para6, uint32_t para7, uint32_t para8, uint32_t para9)
{
    TCIP_OBSD_TRACE_09_EXT(Idx, (LTRC_LEVEL_TYPE)TraceLevel, ModuleID, Line, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9);
}

void obsd_pn_trace_10(uint32_t Idx, uint32_t ModuleID, uint32_t Line, OBSD_LEVEL_TYPE TraceLevel, char* msg, uint32_t para1, uint32_t para2, uint32_t para3, uint32_t para4, uint32_t para5, uint32_t para6, uint32_t para7, uint32_t para8, uint32_t para9, uint32_t para10)
{
    TCIP_OBSD_TRACE_10_EXT(Idx, (LTRC_LEVEL_TYPE)TraceLevel, ModuleID, Line, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10);
}

void obsd_pn_trace_11(uint32_t Idx, uint32_t ModuleID, uint32_t Line, OBSD_LEVEL_TYPE TraceLevel, char* msg, uint32_t para1, uint32_t para2, uint32_t para3, uint32_t para4, uint32_t para5, uint32_t para6, uint32_t para7, uint32_t para8, uint32_t para9, uint32_t para10, uint32_t para11)
{
    TCIP_OBSD_TRACE_11_EXT(Idx, (LTRC_LEVEL_TYPE)TraceLevel, ModuleID, Line, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11);
}

void obsd_pn_trace_12(uint32_t Idx, uint32_t ModuleID, uint32_t Line, OBSD_LEVEL_TYPE TraceLevel, char* msg, uint32_t para1, uint32_t para2, uint32_t para3, uint32_t para4, uint32_t para5, uint32_t para6, uint32_t para7, uint32_t para8, uint32_t para9, uint32_t para10, uint32_t para11, uint32_t para12)
{
    TCIP_OBSD_TRACE_12_EXT(Idx, (LTRC_LEVEL_TYPE)TraceLevel, ModuleID, Line, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12);
}

void obsd_pn_trace_13(uint32_t Idx, uint32_t ModuleID, uint32_t Line, OBSD_LEVEL_TYPE TraceLevel, char* msg, uint32_t para1, uint32_t para2, uint32_t para3, uint32_t para4, uint32_t para5, uint32_t para6, uint32_t para7, uint32_t para8, uint32_t para9, uint32_t para10, uint32_t para11, uint32_t para12, uint32_t para13)
{
    TCIP_OBSD_TRACE_13_EXT(Idx, (LTRC_LEVEL_TYPE)TraceLevel, ModuleID, Line, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12, para13);
}

void obsd_pn_trace_14(uint32_t Idx, uint32_t ModuleID, uint32_t Line, OBSD_LEVEL_TYPE TraceLevel, char* msg, uint32_t para1, uint32_t para2, uint32_t para3, uint32_t para4, uint32_t para5, uint32_t para6, uint32_t para7, uint32_t para8, uint32_t para9, uint32_t para10, uint32_t para11, uint32_t para12, uint32_t para13, uint32_t para14)
{
    TCIP_OBSD_TRACE_14_EXT(Idx, (LTRC_LEVEL_TYPE)TraceLevel, ModuleID, Line, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12, para13, para14);
}

void obsd_pn_trace_15(uint32_t Idx, uint32_t ModuleID, uint32_t Line, OBSD_LEVEL_TYPE TraceLevel, char* msg, uint32_t para1, uint32_t para2, uint32_t para3, uint32_t para4, uint32_t para5, uint32_t para6, uint32_t para7, uint32_t para8, uint32_t para9, uint32_t para10, uint32_t para11, uint32_t para12, uint32_t para13, uint32_t para14, uint32_t para15)
{
    TCIP_OBSD_TRACE_15_EXT(Idx, (LTRC_LEVEL_TYPE)TraceLevel, ModuleID, Line, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12, para13, para14, para15);
}

void obsd_pn_trace_16(uint32_t Idx, uint32_t ModuleID, uint32_t Line, OBSD_LEVEL_TYPE TraceLevel, char* msg, uint32_t para1, uint32_t para2, uint32_t para3, uint32_t para4, uint32_t para5, uint32_t para6, uint32_t para7, uint32_t para8, uint32_t para9, uint32_t para10, uint32_t para11, uint32_t para12, uint32_t para13, uint32_t para14, uint32_t para15, uint32_t para16)
{
    TCIP_OBSD_TRACE_16_EXT(Idx, (LTRC_LEVEL_TYPE)TraceLevel, ModuleID, Line, msg, para1, para2, para3, para4, para5, para6, para7, para8, para9, para10, para11, para12, para13, para14, para15, para16);
}
