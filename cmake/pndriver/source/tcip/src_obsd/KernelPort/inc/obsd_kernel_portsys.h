#ifndef    __OBSD_PORTSYS_H__
#define    __OBSD_PORTSYS_H__

#define	round_page(x)	(((x) + PAGE_MASK) & ~PAGE_MASK)   /* uvm\uvm_param.h */
extern const struct kmem_pa_mode kp_dma_contig;   /* uvm\uvm_km.c */

/* struct umlist_head is defined in obsd_kernel_proc.h via SLIST_HEAD */
extern int AddPendingUserlandMallocs(struct umlist_head * item_list_head);

typedef int     boolean_t;   /* nomally defined in sys\uvm\uvm_param.h */

struct kmem_pa_mode {       /* normally in uvm\uvm_extern.h      */
    int dummy;
};

#endif  /* __OBSD_PORTSYS_H__ */
