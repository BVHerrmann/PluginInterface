/* -------------------------------------------------------------------------------------------------- */
/**
 * @file        obsd_userland_pnio_recvsndfromto.h
 *
 * @brief       interface enhanced sendto and recvfrom functions sendfromto / recvfromto 
 * @details     Interface declarations for the enhanced UDP Packet functions
 *
 * @author      PROFINET Runtime BC / IPPL Team / gh2289n
 *
 * @version     V1.00
 * @date        created: 19.06.2017
 */
/* -------------------------------------------------------------------------------------------------- */

#ifndef __OBSD_USERLAND_PNIO_RECVSNDFROMTO_H__
#define __OBSD_USERLAND_PNIO_RECVSNDFROMTO_H__
extern int obsd_pnio_sendfromto(    int             s,
                                    void            *msg,
                                    unsigned int    len,
                                    int             flags,
                                    const void      *from,
                                    unsigned int    fromlen,
                                    const void      *to,
                                    unsigned int    tolen
                               );

extern int obsd_pnio_recvfromto(    int             s,
                                    void            *buf,
                                    unsigned int    len,
                                    int             flags,
                                    void            *from,
                                    unsigned int    *fromlen,
                                    void            *to,
                                    unsigned int    *tolen,
                                    int             *if_index
                               );

#endif /* __OBSD_USERLAND_PNIO_RECVSNDFROMTO_H__ */

