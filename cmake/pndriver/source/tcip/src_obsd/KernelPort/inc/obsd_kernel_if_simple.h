#ifndef __IF_SIMPLE_H__
#define __IF_SIMPLE_H__

/* softc */
struct obsd_softc 
{
	struct arpcom sc_arpcom; /* interface info */
	struct timeout timer_handle;
	void * tcip_channel; /* the TCIP arp-channel of this interface */
    int    if_is_attached;
};

int obsd_if_init(struct ifnet *ifp);
int obsd_if_attach( struct obsd_softc *sc);
int obsd_if_detach( struct obsd_softc *sc);
void obsd_if_start(struct ifnet *);

#endif /* __IF_SIMPLE_H__ */
