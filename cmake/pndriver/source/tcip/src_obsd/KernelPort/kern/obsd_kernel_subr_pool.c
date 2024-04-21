/*	$OpenBSD: subr_pool.c,v 1.111 2011/11/23 02:05:17 dlg Exp $	*/
/*	$NetBSD: subr_pool.c,v 1.61 2001/09/26 07:14:56 chs Exp $	*/

/*-
 * Copyright (c) 1997, 1999, 2000 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Paul Kranenburg; by Jason R. Thorpe of the Numerical Aerospace
 * Simulation Facility, NASA Ames Research Center.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_subr_pool_act_module) */
#define LTRC_ACT_MODUL_ID 4031 /* OBSD_ITGR added for LSA traces */
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_subr_pool_act_module) */
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
#include <sys/obsd_kernel_proc.h>
#include <sys/obsd_kernel_errno.h>
#include <sys/obsd_kernel_kernel.h>
#include <sys/obsd_kernel_malloc.h>
#include <sys/obsd_kernel_pool.h>
#include <sys/obsd_kernel_syslog.h>
#include <sys/obsd_kernel_sysctl.h>
#include "obsd_kernel_portsys.h"
#include "obsd_platform_osal.h" /* gerlach/gh2289n:  use OSAL memory mgmt instead */
#include <dev/obsd_kernel_rndvar.h>
/* OBSD_TRACE_PATCH_START - patch no. (obsd_include_trace_header) */
#include <tcip_obsd_trc.h>
/* OBSD_TRACE_PATCH_END - patch no. (obsd_include_trace_header) */


/*
 * Pool resource management utility.
 *
 * Memory is allocated in pages which are split into pieces according to
 * the pool item size. Each page is kept on one of three lists in the
 * pool structure: `pr_emptypages', `pr_fullpages' and `pr_partpages',
 * for empty, full and partially-full pages respectively. The individual
 * pool items are on a linked list headed by `ph_itemlist' in each page
 * header. The memory for building the page list is either taken from
 * the allocated pages themselves (for small pool items) or taken from
 * an internal pool of page headers (`phpool').
 */

#define DIAGNOSTIC
/*#define POOL_DEBUG*/

#if defined POOL_DEBUG || defined DDB
int pool_chk(struct pool *pp);
#endif  /* POOL_DEBUG || DDB */

static int phpool_initialized = 0;

/* List of all pools */
TAILQ_HEAD(,pool) pool_head = TAILQ_HEAD_INITIALIZER(pool_head);

/* Private pool for page header structures */
struct pool phpool;

#ifdef OBSD_MEMORY_TRACING
extern struct pool socket_pool;
extern struct pool file_pool;
extern struct pool fdesc_pool;
extern struct pool plimit_pool;
extern struct pool mbpool;
extern struct pool mclpools[7];
extern struct pool ifaddr_item_pl;
extern struct pool rtentry_pool;
extern struct pool rttimer_pool;
extern struct pool inpcb_pool;
extern struct pool ipqent_pool;
extern struct pool ipq_pool;
extern struct pool syn_cache_pool;
extern struct pool tcpcb_pool;
extern struct pool tcpqe_pool;
extern struct pool ucred_pool;
extern struct pool sigacts_pool;
#endif /* OBSD_MEMORY_TRACING */

struct pool_item_header {
	/* Page headers */
	LIST_ENTRY(pool_item_header)
				ph_pagelist;	/* pool page list */
	TAILQ_HEAD(,pool_item)	ph_itemlist;	/* chunk list for this page */
	RB_ENTRY(pool_item_header)
				ph_node;	/* Off-page page headers */
	unsigned int	ph_nmissing;	/* # of chunks in use */ /* gerlach/gh2289n: changed type from int to unsigned int to avoid compiler warning C4389: '==' or '!=' : signed/unsigned mismatch */
	caddr_t			ph_page;	/* this page's address */
	caddr_t			ph_colored;	/* page's colored address */
	int			ph_pagesize;
	int			ph_magic;
};

struct pool_item {
	/* Other entries use only this list entry */
	TAILQ_ENTRY(pool_item)	pi_list;
};

#ifdef DEADBEEF1
#define	PI_MAGIC DEADBEEF1
#else
#define	PI_MAGIC 0xdeafbeef
#endif

int	pool_debug = 0;

#define	POOL_NEEDS_CATCHUP(pp)						\
	((pp)->pr_nitems < (pp)->pr_minitems)

/*
 * Every pool gets a unique serial number assigned to it. If this counter
 * wraps, we're screwed, but we shouldn't create so many pools anyway.
 */
unsigned int pool_serial = 0;	/* mh2290: LINT 729   Symbol not explicitly initialized */

int	 pool_catchup(struct pool *);
void	 pool_prime_page(struct pool *, caddr_t, struct pool_item_header *);
void	 pool_update_curpage(struct pool *);
void	*pool_do_get(struct pool *, int);
void	 pool_do_put(struct pool *, void *);
void	 pr_rmpage(struct pool *, struct pool_item_header *,
	    struct pool_pagelist *);
//int	pool_chk_page(struct pool *, struct pool_item_header *, int); /* comment out the declaration to avoid the warning from compiler "i586-poky-linux-gcc": Unused declaration of function */
struct pool_item_header *pool_alloc_item_header(struct pool *, caddr_t , int);

void	*pool_allocator_alloc(struct pool *, int, int *);
void	 pool_allocator_free(struct pool *, void *);

/*
 * XXX - quick hack. For pools with large items we want to use a special
 *       allocator. For now, instead of having the allocator figure out
 *       the allocation size from the pool (which can be done trivially
 *       with round_page(pr_itemsperpage * pr_size)) which would require
 *	 lots of changes everywhere, we just create allocators for each
 *	 size. We limit those to 128 pages.
 */
#define POOL_LARGE_MAXPAGES 128
struct pool_allocator pool_allocator_large[POOL_LARGE_MAXPAGES];
struct pool_allocator pool_allocator_large_ni[POOL_LARGE_MAXPAGES];
void	*pool_large_alloc(struct pool *, int, int *);
void	pool_large_free(struct pool *, void *);
void	*pool_large_alloc_ni(struct pool *, int, int *);
void	pool_large_free_ni(struct pool *, void *);

#if OBSD_PNIO_CFG_TRACE_ON
extern int tcps_reass;		/* tcp statistics */
#endif /* OBSD_PNIO_CFG_TRACE_ON */

#ifdef OBSD_MEMORY_TRACING
void pool_print_pagelist(struct pool_pagelist *pl)
{
		struct pool_item_header *ph;

		LIST_FOREACH(ph, pl, ph_pagelist)
		{
			TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_CHAT, "page 0x%x", (uint32_t) ph->ph_page);
		}
}

void pool_print_details(struct pool *pl)
{
	if (pl == &socket_pool) { TCIP_OBSD_TRACE_08(0, OBSD_TRACE_LEVEL_FATAL,			"| %2u | socket_pool    | %5u | %5u | %10u | %5u | %5u | %5u | %5u |", pl->pr_serial, pl->pr_nout, pl->pr_ihiwat, pl->pr_hardlimit, pl->pr_npages, pl->pr_nidle, pl->pr_minpages, pl->pr_hiwat);}
	else if (pl == &file_pool) {TCIP_OBSD_TRACE_08(0, OBSD_TRACE_LEVEL_FATAL,		"| %2u | file_pool      | %5u | %5u | %10u | %5u | %5u | %5u | %5u |", pl->pr_serial, pl->pr_nout, pl->pr_ihiwat, pl->pr_hardlimit, pl->pr_npages, pl->pr_nidle, pl->pr_minpages, pl->pr_hiwat);}
	else if (pl == &fdesc_pool) { TCIP_OBSD_TRACE_08(0, OBSD_TRACE_LEVEL_FATAL,		"| %2u | fdesc_pool     | %5u | %5u | %10u | %5u | %5u | %5u | %5u |", pl->pr_serial, pl->pr_nout, pl->pr_ihiwat, pl->pr_hardlimit, pl->pr_npages, pl->pr_nidle, pl->pr_minpages, pl->pr_hiwat);}
	else if (pl == &plimit_pool) { TCIP_OBSD_TRACE_08(0, OBSD_TRACE_LEVEL_FATAL,	"| %2u | plimit_pool    | %5u | %5u | %10u | %5u | %5u | %5u | %5u |", pl->pr_serial, pl->pr_nout, pl->pr_ihiwat, pl->pr_hardlimit, pl->pr_npages, pl->pr_nidle, pl->pr_minpages, pl->pr_hiwat);}
	else if (pl == &mbpool) { TCIP_OBSD_TRACE_08(0, OBSD_TRACE_LEVEL_FATAL,			"| %2u | mbpool         | %5u | %5u | %10u | %5u | %5u | %5u | %5u |", pl->pr_serial, pl->pr_nout, pl->pr_ihiwat, pl->pr_hardlimit, pl->pr_npages, pl->pr_nidle, pl->pr_minpages, pl->pr_hiwat);}
	else if (pl == &mclpools[0]) { TCIP_OBSD_TRACE_08(0, OBSD_TRACE_LEVEL_FATAL,	"| %2u | mclpools[0]    | %5u | %5u | %10u | %5u | %5u | %5u | %5u |", pl->pr_serial, pl->pr_nout, pl->pr_ihiwat, pl->pr_hardlimit, pl->pr_npages, pl->pr_nidle, pl->pr_minpages, pl->pr_hiwat);}
	else if (pl == &mclpools[1]) { TCIP_OBSD_TRACE_08(0, OBSD_TRACE_LEVEL_FATAL,	"| %2u | mclpools[1]    | %5u | %5u | %10u | %5u | %5u | %5u | %5u |", pl->pr_serial, pl->pr_nout, pl->pr_ihiwat, pl->pr_hardlimit, pl->pr_npages, pl->pr_nidle, pl->pr_minpages, pl->pr_hiwat);}
	else if (pl == &mclpools[2]) { TCIP_OBSD_TRACE_08(0, OBSD_TRACE_LEVEL_FATAL,	"| %2u | mclpools[2]    | %5u | %5u | %10u | %5u | %5u | %5u | %5u |", pl->pr_serial, pl->pr_nout, pl->pr_ihiwat, pl->pr_hardlimit, pl->pr_npages, pl->pr_nidle, pl->pr_minpages, pl->pr_hiwat);}
	else if (pl == &mclpools[3]) { TCIP_OBSD_TRACE_08(0, OBSD_TRACE_LEVEL_FATAL,	"| %2u | mclpools[3]    | %5u | %5u | %10u | %5u | %5u | %5u | %5u |", pl->pr_serial, pl->pr_nout, pl->pr_ihiwat, pl->pr_hardlimit, pl->pr_npages, pl->pr_nidle, pl->pr_minpages, pl->pr_hiwat);}
	else if (pl == &mclpools[4]) { TCIP_OBSD_TRACE_08(0, OBSD_TRACE_LEVEL_FATAL,	"| %2u | mclpools[4]    | %5u | %5u | %10u | %5u | %5u | %5u | %5u |", pl->pr_serial, pl->pr_nout, pl->pr_ihiwat, pl->pr_hardlimit, pl->pr_npages, pl->pr_nidle, pl->pr_minpages, pl->pr_hiwat);}
	else if (pl == &mclpools[5]) { TCIP_OBSD_TRACE_08(0, OBSD_TRACE_LEVEL_FATAL,	"| %2u | mclpools[5]    | %5u | %5u | %10u | %5u | %5u | %5u | %5u |", pl->pr_serial, pl->pr_nout, pl->pr_ihiwat, pl->pr_hardlimit, pl->pr_npages, pl->pr_nidle, pl->pr_minpages, pl->pr_hiwat);}
	else if (pl == &mclpools[6]) { TCIP_OBSD_TRACE_08(0, OBSD_TRACE_LEVEL_FATAL,	"| %2u | mclpools[6]    | %5u | %5u | %10u | %5u | %5u | %5u | %5u |", pl->pr_serial, pl->pr_nout, pl->pr_ihiwat, pl->pr_hardlimit, pl->pr_npages, pl->pr_nidle, pl->pr_minpages, pl->pr_hiwat);}
	else if (pl == &ifaddr_item_pl) { TCIP_OBSD_TRACE_08(0, OBSD_TRACE_LEVEL_FATAL,	"| %2u | ifaddr_item_pl | %5u | %5u | %10u | %5u | %5u | %5u | %5u |", pl->pr_serial, pl->pr_nout, pl->pr_ihiwat, pl->pr_hardlimit, pl->pr_npages, pl->pr_nidle, pl->pr_minpages, pl->pr_hiwat);}
	else if (pl == &rtentry_pool) { TCIP_OBSD_TRACE_08(0, OBSD_TRACE_LEVEL_FATAL,	"| %2u | rtentry_pool   | %5u | %5u | %10u | %5u | %5u | %5u | %5u |", pl->pr_serial, pl->pr_nout, pl->pr_ihiwat, pl->pr_hardlimit, pl->pr_npages, pl->pr_nidle, pl->pr_minpages, pl->pr_hiwat);}
	else if (pl == &rttimer_pool){ TCIP_OBSD_TRACE_08(0, OBSD_TRACE_LEVEL_FATAL,	"| %2u | rttimer_pool   | %5u | %5u | %10u | %5u | %5u | %5u | %5u |", pl->pr_serial, pl->pr_nout, pl->pr_ihiwat, pl->pr_hardlimit, pl->pr_npages, pl->pr_nidle, pl->pr_minpages, pl->pr_hiwat);}
	else if (pl == &inpcb_pool) { TCIP_OBSD_TRACE_08(0, OBSD_TRACE_LEVEL_FATAL,		"| %2u | inpcb_pool     | %5u | %5u | %10u | %5u | %5u | %5u | %5u |", pl->pr_serial, pl->pr_nout, pl->pr_ihiwat, pl->pr_hardlimit, pl->pr_npages, pl->pr_nidle, pl->pr_minpages, pl->pr_hiwat);}
	else if (pl == &ipqent_pool) { TCIP_OBSD_TRACE_08(0, OBSD_TRACE_LEVEL_FATAL,	"| %2u | ipqent_pool    | %5u | %5u | %10u | %5u | %5u | %5u | %5u |", pl->pr_serial, pl->pr_nout, pl->pr_ihiwat, pl->pr_hardlimit, pl->pr_npages, pl->pr_nidle, pl->pr_minpages, pl->pr_hiwat);}
	else if (pl == &ipq_pool) { TCIP_OBSD_TRACE_08(0, OBSD_TRACE_LEVEL_FATAL,		"| %2u | ipq_pool       | %5u | %5u | %10u | %5u | %5u | %5u | %5u |", pl->pr_serial, pl->pr_nout, pl->pr_ihiwat, pl->pr_hardlimit, pl->pr_npages, pl->pr_nidle, pl->pr_minpages, pl->pr_hiwat);}
	else if (pl == &syn_cache_pool) { TCIP_OBSD_TRACE_08(0, OBSD_TRACE_LEVEL_FATAL, "| %2u | syn_cache_pool | %5u | %5u | %10u | %5u | %5u | %5u | %5u |", pl->pr_serial, pl->pr_nout, pl->pr_ihiwat, pl->pr_hardlimit, pl->pr_npages, pl->pr_nidle, pl->pr_minpages, pl->pr_hiwat);}
	else if (pl == &tcpcb_pool) { TCIP_OBSD_TRACE_08(0, OBSD_TRACE_LEVEL_FATAL,		"| %2u | tcpcb_pool     | %5u | %5u | %10u | %5u | %5u | %5u | %5u |", pl->pr_serial, pl->pr_nout, pl->pr_ihiwat, pl->pr_hardlimit, pl->pr_npages, pl->pr_nidle, pl->pr_minpages, pl->pr_hiwat);}
	else if (pl == &tcpqe_pool) { TCIP_OBSD_TRACE_08(0, OBSD_TRACE_LEVEL_FATAL,		"| %2u | tcpqe_pool     | %5u | %5u | %10u | %5u | %5u | %5u | %5u |", pl->pr_serial, pl->pr_nout, pl->pr_ihiwat, pl->pr_hardlimit, pl->pr_npages, pl->pr_nidle, pl->pr_minpages, pl->pr_hiwat);}
	else if (pl == &ucred_pool) { TCIP_OBSD_TRACE_08(0, OBSD_TRACE_LEVEL_FATAL,		"| %2u | ucred_pool     | %5u | %5u | %10u | %5u | %5u | %5u | %5u |", pl->pr_serial, pl->pr_nout, pl->pr_ihiwat, pl->pr_hardlimit, pl->pr_npages, pl->pr_nidle, pl->pr_minpages, pl->pr_hiwat);}
	else if (pl == &sigacts_pool) { TCIP_OBSD_TRACE_08(0, OBSD_TRACE_LEVEL_FATAL,	"| %2u | sigacts_pool   | %5u | %5u | %10u | %5u | %5u | %5u | %5u |", pl->pr_serial, pl->pr_nout, pl->pr_ihiwat, pl->pr_hardlimit, pl->pr_npages, pl->pr_nidle, pl->pr_minpages, pl->pr_hiwat);}
	else if (pl == &phpool) { TCIP_OBSD_TRACE_08(0, OBSD_TRACE_LEVEL_FATAL,			"| %2u | phpool         | %5u | %5u | %10u | %5u | %5u | %5u | %5u |", pl->pr_serial, pl->pr_nout, pl->pr_ihiwat, pl->pr_hardlimit, pl->pr_npages, pl->pr_nidle, pl->pr_minpages, pl->pr_hiwat);}
	else { TCIP_OBSD_TRACE_08(0, OBSD_TRACE_LEVEL_FATAL,							"| %2u | unknown        | %5u | %5u | %10u | %5u | %5u  |%5u | %5u |", pl->pr_serial, pl->pr_nout, pl->pr_ihiwat, pl->pr_hardlimit, pl->pr_npages, pl->pr_nidle, pl->pr_minpages, pl->pr_hiwat);}
}

void pool_statistic(void)
{
	struct pool *pp;
	TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_FATAL, "-----------------------------------Pool Statistic----------------------------------");
	TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_FATAL, "|         Info        |             Items          |            Pages              |");
	TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_FATAL, "| Nr | Pool Name      | Alloc | HiWat |  Hardlimit | Alloc |  Idle | LoWat | HiWat |");
	TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_FATAL, "------------------------------------------------------------------------------------");

	TAILQ_FOREACH(pp, &pool_head, pr_poollist)
	{
		pool_print_details(pp);
	}

	TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_FATAL, "----------------------------------End Pool Statistic--------------------------------");

#if OBSD_PNIO_CFG_TRACE_ON
	TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_FATAL, "Number of reassembled packets: %u", tcps_reass);
	TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_FATAL, "------------------------------------------------------------------------------------");
#endif /* OBSD_PNIO_CFG_TRACE_ON */
}

#endif /* OBSD_MEMORY_TRACING */

#define pool_sleep(pl) 	panic("pool sleep")

static __inline int
phtree_compare(struct pool_item_header *a, struct pool_item_header *b)
{
	long diff = (vaddr_t)a->ph_page - (vaddr_t)b->ph_page;
	if (diff < 0)
		return -(-diff >= a->ph_pagesize);
	else if (diff > 0)
		return (diff >= b->ph_pagesize);
	else
		return (0);
}

RB_PROTOTYPE(phtree, pool_item_header, ph_node, phtree_compare);
RB_GENERATE(phtree, pool_item_header, ph_node, phtree_compare);

/*
 * Return the pool page header based on page address.
 */
static __inline struct pool_item_header *
pr_find_pagehead(struct pool *pp, void *v)
{
	struct pool_item_header *ph, tmp;

	if ((pp->pr_roflags & PR_PHINPAGE) != 0) {
		caddr_t page;
		page = (caddr_t)((vaddr_t)v & pp->pr_alloc->pa_pagemask);
		return ((struct pool_item_header *)(page + pp->pr_phoffset));
	}

	/*
	 * The trick we're using in the tree compare function is to compare
	 * two elements equal when they overlap. We want to return the
	 * page header that belongs to the element just before this address.
	 * We don't want this element to compare equal to the next element,
	 * so the compare function takes the pagesize from the lower element.
	 * If this header is the lower, its pagesize is zero, so it can't
	 * overlap with the next header. But if the header we're looking for
	 * is lower, we'll use its pagesize and it will overlap and return
	 * equal.
	 */
	tmp.ph_page = v;
	tmp.ph_pagesize = 0;
	ph = RB_FIND(phtree, &pp->pr_phtree, &tmp);

	if (ph) {
		KASSERT(ph->ph_page <= (caddr_t)v);
		KASSERT(ph->ph_page + ph->ph_pagesize > (caddr_t)v);
	}
	return ph;
}

/*
 * Remove a page from the pool.
 */
void
pr_rmpage(struct pool *pp, struct pool_item_header *ph,
    struct pool_pagelist *pq)
{

	/*
	 * If the page was idle, decrement the idle page count.
	 */
	if (ph->ph_nmissing == 0) {
		pp->pr_nidle--;
	}

	pp->pr_nitems -= pp->pr_itemsperpage;

	/*
	 * Unlink a page from the pool and release it (or queue it for release).
	 */
	LIST_REMOVE(ph, ph_pagelist);
	if ((pp->pr_roflags & PR_PHINPAGE) == 0)
		RB_REMOVE(phtree, &pp->pr_phtree, ph);
	pp->pr_npages--;
	pp->pr_npagefree++;
	pool_update_curpage(pp);

	if (pq) {
		LIST_INSERT_HEAD(pq, ph, ph_pagelist);
	} else {
		pool_allocator_free(pp, ph->ph_page);
		if ((pp->pr_roflags & PR_PHINPAGE) == 0)
			pool_put(&phpool, ph);
	}
}

void pool_management_deinitialize(void)
{
	if (phpool_initialized)
	{
		pool_flush_pages(&phpool);
		pool_destroy(    &phpool);
	}
}


/*
 * Initialize the given pool resource structure.
 *
 * We export this routine to allow other kernel parts to declare
 * static pools that must be initialized before malloc() is available.
 */
void
pool_init(struct pool *pp, size_t size, u_int align, u_int ioff, int flags,
    const char *wchan, struct pool_allocator *palloc)
{
	int off, slack;

	/*
	 * Check arguments and construct default values.
	 */
	if (palloc == NULL) {
		if (size > PAGE_SIZE) {
			int psize;

			/*
			 * XXX - should take align into account as well.
			 */
			if (size == round_page(size))
				psize = (int)(size / PAGE_SIZE); /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4267 */
			else
				psize = (int)(PAGE_SIZE / roundup(size % PAGE_SIZE,  /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4267 */
				    1024));
			if (psize > POOL_LARGE_MAXPAGES)
				psize = POOL_LARGE_MAXPAGES;
			if (flags & PR_WAITOK)
				palloc = &pool_allocator_large_ni[psize-1];
			else
				palloc = &pool_allocator_large[psize-1];
			if (palloc->pa_pagesz == 0) {
				palloc->pa_pagesz = psize * PAGE_SIZE;
				if (flags & PR_WAITOK) {
					palloc->pa_alloc = pool_large_alloc_ni;
					palloc->pa_free = pool_large_free_ni;
				} else {
					palloc->pa_alloc = pool_large_alloc;
					palloc->pa_free = pool_large_free;
				}
			}
		} else {
			palloc = &pool_allocator_nointr;
		}
	}
	if (palloc->pa_pagesz == 0) {
		palloc->pa_pagesz = PAGE_SIZE;
	}
	if (palloc->pa_pagemask == 0) {
		palloc->pa_pagemask = ~(palloc->pa_pagesz - 1);
		palloc->pa_pageshift = ffs(palloc->pa_pagesz) - 1;
	}

	if (align == 0)
		align = ALIGN(1);

	if (size < sizeof(struct pool_item))
		size = sizeof(struct pool_item);

	size = roundup(size, align);

	/*
	 * Initialize the pool structure.
	 */
	LIST_INIT(&pp->pr_emptypages);
	LIST_INIT(&pp->pr_fullpages);
	LIST_INIT(&pp->pr_partpages);
	pp->pr_curpage = NULL;
	pp->pr_npages = 0;
	pp->pr_minitems = 0;
	pp->pr_minpages = 0;
    pp->pr_maxpages = 2;                /* pools that don't explicitly set watermarks get only 2 pages              */
	pp->pr_roflags = flags;
	pp->pr_flags = 0;
	pp->pr_size = (unsigned int)size;  /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4267 */
	pp->pr_align = align;
	pp->pr_wchan = wchan;
	pp->pr_alloc = palloc;
	pp->pr_nitems = 0;
	pp->pr_nout = 0;
#ifdef OBSD_MEMORY_TRACING
	pp->pr_ihiwat = 0;
#endif /* OBSD_MEMORY_TRACING */
	pp->pr_hardlimit = UINT_MAX;
	pp->pr_hardlimit_warning = NULL;
	pp->pr_hardlimit_ratecap.tv_sec = 0;
	pp->pr_hardlimit_ratecap.tv_usec = 0;
	pp->pr_hardlimit_warning_last.tv_sec = 0;
	pp->pr_hardlimit_warning_last.tv_usec = 0;
	pp->pr_serial = ++pool_serial;
	if (pool_serial == 0)
	{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_subr_pool_001) */
		TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_FATAL, "pool_init: too much uptime");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_subr_pool_001) */
		panic("pool_init: too much uptime");
	}

        /* constructor, destructor, and arg */
	pp->pr_ctor = NULL;
	pp->pr_dtor = NULL;
	pp->pr_arg = NULL;

	/*
	 * Decide whether to put the page header off page to avoid
	 * wasting too large a part of the page. Off-page page headers
	 * go into an RB tree, so we can match a returned item with
	 * its header based on the page address.
	 * We use 1/16 of the page size as the threshold (XXX: tune)
	 */
	if (pp->pr_size < palloc->pa_pagesz/16 && pp->pr_size < PAGE_SIZE) {
		/* Use the end of the page for the page header */
		pp->pr_roflags |= PR_PHINPAGE;
		pp->pr_phoffset = off = palloc->pa_pagesz -
		    ALIGN(sizeof(struct pool_item_header));
	} else {
		/* The page header will be taken from our page header pool */
		pp->pr_phoffset = 0;
		off = palloc->pa_pagesz;
		RB_INIT(&pp->pr_phtree);
	}

	/*
	 * Alignment is to take place at `ioff' within the item. This means
	 * we must reserve up to `align - 1' bytes on the page to allow
	 * appropriate positioning of each item.
	 *
	 * Silently enforce `0 <= ioff < align'.
	 */
	pp->pr_itemoffset = ioff = ioff % align;
	pp->pr_itemsperpage = (off - ((align - ioff) % align)) / pp->pr_size;
	KASSERT(pp->pr_itemsperpage != 0);

	/*
	 * Use the slack between the chunks and the page header
	 * for "cache coloring".
	 */
	slack = off - pp->pr_itemsperpage * pp->pr_size;
	pp->pr_maxcolor = (slack / align) * align;
	pp->pr_curcolor = 0;

	pp->pr_nget = 0;
	pp->pr_nfail = 0;
	pp->pr_nput = 0;
	pp->pr_npagealloc = 0;
	pp->pr_npagefree = 0;
	pp->pr_hiwat = 0;
	pp->pr_nidle = 0;

	pp->pr_ipl = -1;

	if (phpool.pr_size == 0) {
		pool_init(&phpool, sizeof(struct pool_item_header), 0, 0,
		    0, "phpool", NULL);
		pool_setipl(&phpool, IPL_HIGH);
		phpool_initialized = 1;
	}

	/* Insert this into the list of all pools. */
	TAILQ_INSERT_HEAD(&pool_head, pp, pr_poollist);
}

void
pool_setipl(struct pool *pp, int ipl)
{
	pp->pr_ipl = ipl;
}

/*
 * Decommission a pool resource.
 */
void
pool_destroy(struct pool *pp)
{
	struct pool_item_header *ph;

	/* Remove all pages */
	while ((ph = LIST_FIRST(&pp->pr_emptypages)) != NULL)
		pr_rmpage(pp, ph, NULL);
	KASSERT(LIST_EMPTY(&pp->pr_fullpages));
	KASSERT(LIST_EMPTY(&pp->pr_partpages));

	/* Remove from global pool list */
	if(TAILQ_FIRST(&pool_head) != NULL)
	TAILQ_REMOVE(&pool_head, pp, pr_poollist);
}


struct pool_item_header *
pool_alloc_item_header(struct pool *pp, caddr_t storage, int flags)
{
	struct pool_item_header *ph;

	if ((pp->pr_roflags & PR_PHINPAGE) != 0)
		ph = (struct pool_item_header *)(storage + pp->pr_phoffset);
	else
		ph = pool_get(&phpool, (flags & ~(PR_WAITOK | PR_ZERO)) |
		    PR_NOWAIT);
	if (pool_debug && ph != NULL)
		ph->ph_magic = PI_MAGIC;
	return (ph);
}


/*
 * Grab an item from the pool; must be called at appropriate spl level
 */
void *
pool_get(struct pool *pp, int flags)
{
	void *v;

	KASSERT(flags & (PR_WAITOK | PR_NOWAIT));
	v = pool_do_get(pp, flags);
	if (v != NULL)
		pp->pr_nget++;
	if (v == NULL)
		return (v);

	if (pp->pr_ctor) {
		if (flags & PR_ZERO)
		{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_subr_pool_002) */
			TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_FATAL, "pool_get: PR_ZERO when ctor set");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_subr_pool_002) */
			panic("pool_get: PR_ZERO when ctor set");
		}
		if (pp->pr_ctor(pp->pr_arg, v, flags)) {
			pp->pr_nget--;
			pool_do_put(pp, v);
			v = NULL;
		}
	} else {
		if (flags & PR_ZERO)
			memset(v, 0, pp->pr_size);
	}
	return (v);
}


void *
pool_do_get(struct pool *pp, int flags)
{
	struct pool_item *pi;
	struct pool_item_header *ph;
	void *v;
	int slowdown = 0;
#if defined(DIAGNOSTIC) && defined(POOL_DEBUG)
	int i, *ip;
#endif

startover:
	/*
	 * Check to see if we've reached the hard limit.  If we have,
	 * and we can wait, then wait until an item has been returned to
	 * the pool.
	 */
	if (pp->pr_nout == pp->pr_hardlimit) {
		if ((flags & PR_WAITOK) && !(flags & PR_LIMITFAIL)) {
			/*
			 * XXX: A warning isn't logged in this case.  Should
			 * it be?
			 */
			pp->pr_flags |= PR_WANTED;
			pool_sleep(pp);
			goto startover;
		}

		/*
		 * Log a message that the hard limit has been hit.
		 */
		if (pp->pr_hardlimit_warning != NULL &&
		    ratecheck(&pp->pr_hardlimit_warning_last,
		    &pp->pr_hardlimit_ratecap))
		{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_subr_pool_003) */
			TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_ERROR, "hard limit has been hit");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_subr_pool_003) */
			log(LOG_ERR, "%s\n", pp->pr_hardlimit_warning);
		}

		pp->pr_nfail++;
		return (NULL);
	}

	/*
	 * The convention we use is that if `curpage' is not NULL, then
	 * it points at a non-empty bucket. In particular, `curpage'
	 * never points at a page header which has PR_PHINPAGE set and
	 * has no items in its bucket.
	 */
	if ((ph = pp->pr_curpage) == NULL) {
		/*
		 * Call the back-end page allocator for more memory.
		 */
		v = pool_allocator_alloc(pp, flags, &slowdown);
		if (v != NULL)
			ph = pool_alloc_item_header(pp, v, flags);

		if (v == NULL || ph == NULL) {
			if (v != NULL)
				pool_allocator_free(pp, v);

			if ((flags & PR_WAITOK) == 0) {
				pp->pr_nfail++;
				return (NULL);
			}

			/*
			 * Wait for items to be returned to this pool.
			 *
			 * XXX: maybe we should wake up once a second and
			 * try again?
			 */
			pp->pr_flags |= PR_WANTED;
			pool_sleep(pp);
			goto startover;
		}

		/* We have more memory; add it to the pool */
		pool_prime_page(pp, v, ph);
		pp->pr_npagealloc++;

		/* Start the allocation process over. */
		goto startover;
	}
	if ((v = pi = TAILQ_FIRST(&ph->ph_itemlist)) == NULL) {
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_subr_pool_004) */
		TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_FATAL, "pool_do_get: page empty");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_subr_pool_004) */
		panic("pool_do_get: %s: page empty", pp->pr_wchan);
	}

	/*
	 * Remove from item list.
	 */
	TAILQ_REMOVE(&ph->ph_itemlist, pi, pi_list);
	pp->pr_nitems--;
	pp->pr_nout++;
#ifdef OBSD_MEMORY_TRACING
	if (pp->pr_nout > pp->pr_ihiwat) 
	{
		pp->pr_ihiwat = pp->pr_nout;
	}
#endif /* OBSD_MEMORY_TRACING */

	if (ph->ph_nmissing == 0) {
		pp->pr_nidle--;

		/*
		 * This page was previously empty.  Move it to the list of
		 * partially-full pages.  This page is already curpage.
		 */
		LIST_REMOVE(ph, ph_pagelist);
		LIST_INSERT_HEAD(&pp->pr_partpages, ph, ph_pagelist);
	}
	ph->ph_nmissing++;
	if (TAILQ_EMPTY(&ph->ph_itemlist)) {
		/*
		 * This page is now full.  Move it to the full list
		 * and select a new current page.
		 */
		LIST_REMOVE(ph, ph_pagelist);
		LIST_INSERT_HEAD(&pp->pr_fullpages, ph, ph_pagelist);
		pool_update_curpage(pp);
	}

	/*
	 * If we have a low water mark and we are now below that low
	 * water mark, add more items to the pool.
	 */
	if (POOL_NEEDS_CATCHUP(pp) && pool_catchup(pp) != 0) {
		/*
		 * XXX: Should we log a warning?  Should we set up a timeout
		 * to try again in a second or so?  The latter could break
		 * a caller's assumptions about interrupt protection, etc.
		 */
	}
#ifdef    BSD_STACKPORT_POOL_DIAG    /* gerlach/gh2289n: additional pool debug code*/
    {
        caddr_t page = (caddr_t)((u_long)ph & pp->pr_alloc->pa_pagemask);
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_subr_pool_005) */
        TCIP_OBSD_TRACE_03(0, OBSD_TRACE_LEVEL_CHAT, "pool_do_get, v=%p pool(%p), page %p", v, pp, ph->ph_page);
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_subr_pool_005) */
        printf("pool_do_get, v=%p pool(%p:%s), page %p head addr %p (p %p)\n", v, pp, pp->pr_wchan, ph->ph_page, ph, page);
    }
#endif /* BSD_STACKPORT_POOL_DIAG */ /* gerlach/gh2289n */

	return (v);
}



/*
 * Return resource to the pool; must be called at appropriate spl level
 */
void
pool_put(struct pool *pp, void *v)
{
	if (pp->pr_dtor)
		pp->pr_dtor(pp->pr_arg, v);
	pool_do_put(pp, v);
	pp->pr_nput++;
}



/*
 * Internal version of pool_put().
 */
void
pool_do_put(struct pool *pp, void *v)
{
	struct pool_item *pi = v;
	struct pool_item_header *ph;
#if defined(DIAGNOSTIC) && defined(POOL_DEBUG)
	int i, *ip;
#endif

	if (v == NULL)
	{
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_subr_pool_006) */
		TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_FATAL, "pool_put of NULL");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_subr_pool_006) */
		panic("pool_put of NULL");
	}

	if ((ph = pr_find_pagehead(pp, v)) == NULL) {
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_subr_pool_007) */
		TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_FATAL, "pool_do_put: page header missing");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_subr_pool_007) */
		panic("pool_do_put: %s: page header missing", pp->pr_wchan);
	}

#ifdef    BSD_STACKPORT_POOL_DIAG    /* gerlach/gh2289n: additional pool debug code*/
    {
        caddr_t page = (caddr_t)((u_long)ph & pp->pr_alloc->pa_pagemask);
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_subr_pool_008) */
        TCIP_OBSD_TRACE_03(0, OBSD_TRACE_LEVEL_CHAT, "pool_do_put v=%p pool(%p) page=%p", v, pp, page);
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_subr_pool_008) */
        printf("pool_do_put v=%p pool(%p:%s) ph_page=%p ph=%p page=%p mask=0x%08lx\n", v, pp, pp->pr_wchan, ph->ph_page, ph, page, pp->pr_alloc->pa_pagemask);
    }
#endif /* BSD_STACKPORT_POOL_DIAG */ /* gerlach/gh2289n: */
	/*
	 * Return to item list.
	 */

	TAILQ_INSERT_HEAD(&ph->ph_itemlist, pi, pi_list);
	ph->ph_nmissing--;
	pp->pr_nitems++;
	pp->pr_nout--;

	/* Cancel "pool empty" condition if it exists */
	if (pp->pr_curpage == NULL)
		pp->pr_curpage = ph;

	if (pp->pr_flags & PR_WANTED) {
		pp->pr_flags &= ~PR_WANTED;
		wakeup(pp);
	}

	/*
	 * If this page is now empty, do one of two things:
	 *
	 *	(1) If we have more pages than the page high water mark,
	 *	    free the page back to the system.
	 *
	 *	(2) Otherwise, move the page to the empty page list.
	 *
	 * Either way, select a new current page (so we use a partially-full
	 * page if one is available).
	 */
	if (ph->ph_nmissing == 0) {
		pp->pr_nidle++;
		if (pp->pr_nidle > pp->pr_maxpages) {
			pr_rmpage(pp, ph, NULL);
		} else {
			LIST_REMOVE(ph, ph_pagelist);
			LIST_INSERT_HEAD(&pp->pr_emptypages, ph, ph_pagelist);
			pool_update_curpage(pp);
		}
	}

	/*
	 * If the page was previously completely full, move it to the
	 * partially-full list and make it the current page.  The next
	 * allocation will get the item from this page, instead of
	 * further fragmenting the pool.
	 */
	else if (ph->ph_nmissing == (pp->pr_itemsperpage - 1)) {
		LIST_REMOVE(ph, ph_pagelist);
		LIST_INSERT_HEAD(&pp->pr_partpages, ph, ph_pagelist);
		pp->pr_curpage = ph;
	}
}

/*
 * Add a page worth of items to the pool.
 *
 * Note, we must be called with the pool descriptor LOCKED.
 */
void
pool_prime_page(struct pool *pp, caddr_t storage, struct pool_item_header *ph)
{
	struct pool_item *pi;
	caddr_t cp = storage;
	unsigned int align = pp->pr_align;
	unsigned int ioff = pp->pr_itemoffset;
	int n;
#if defined(DIAGNOSTIC) && defined(POOL_DEBUG)
	int i, *ip;
#endif

	/*
	 * Insert page header.
	 */
	LIST_INSERT_HEAD(&pp->pr_emptypages, ph, ph_pagelist);
	TAILQ_INIT(&ph->ph_itemlist);
	ph->ph_page = storage;
	ph->ph_pagesize = pp->pr_alloc->pa_pagesz;
	ph->ph_nmissing = 0;
	if ((pp->pr_roflags & PR_PHINPAGE) == 0)
		RB_INSERT(phtree, &pp->pr_phtree, ph);

	pp->pr_nidle++;

	/*
	 * Color this page.
	 */
	cp = (caddr_t)(cp + pp->pr_curcolor);
	if ((pp->pr_curcolor += align) > pp->pr_maxcolor)
		pp->pr_curcolor = 0;

	/*
	 * Adjust storage to apply alignment to `pr_itemoffset' in each item.
	 */
	if (ioff != 0)
		cp = (caddr_t)(cp + (align - ioff));
	ph->ph_colored = cp;

	/*
	 * Insert remaining chunks on the bucket list.
	 */
	n = pp->pr_itemsperpage;
	pp->pr_nitems += n;

	while (n--) {
		pi = (struct pool_item *)cp;

		KASSERT(((((vaddr_t)pi) + ioff) & (align - 1)) == 0);

		/* Insert on page list */
		TAILQ_INSERT_TAIL(&ph->ph_itemlist, pi, pi_list);

		cp = (caddr_t)(cp + pp->pr_size);
	}

	/*
	 * If the pool was depleted, point at the new page.
	 */
	if (pp->pr_curpage == NULL)
		pp->pr_curpage = ph;

	if (++pp->pr_npages > pp->pr_hiwat)
		pp->pr_hiwat = pp->pr_npages;
}


/*
 * Used by pool_get() when nitems drops below the low water mark.  This
 * is used to catch up pr_nitems with the low water mark.
 *
 * Note we never wait for memory here, we let the caller decide what to do.
 */
int
pool_catchup(struct pool *pp)
{
	struct pool_item_header *ph = NULL; /* gerlach/gh2289n: added init value to avoid compiler warning C4701: potentially uninitialized local variable used */
	caddr_t cp;
	int error = 0;
	int slowdown = 0;

	while (POOL_NEEDS_CATCHUP(pp)) {
		/*
		 * Call the page back-end allocator for more memory.
		 */
		cp = pool_allocator_alloc(pp, PR_NOWAIT, &slowdown);
		if (cp != NULL)
			ph = pool_alloc_item_header(pp, cp, PR_NOWAIT);
		if (cp == NULL || ph == NULL) {
			if (cp != NULL)
				pool_allocator_free(pp, cp);
			error = ENOMEM;
			break;
		}
		pool_prime_page(pp, cp, ph);
		pp->pr_npagealloc++;
	}

	return (error);
}

void
pool_update_curpage(struct pool *pp)
{

	pp->pr_curpage = LIST_FIRST(&pp->pr_partpages);
	if (pp->pr_curpage == NULL) {
		pp->pr_curpage = LIST_FIRST(&pp->pr_emptypages);
	}
}


void
pool_setlowat(struct pool *pp, int n)
{
	pp->pr_minitems = n;
	pp->pr_minpages = (n == 0)
		? 0
		: roundup(n, pp->pr_itemsperpage) / pp->pr_itemsperpage;

	/* Make sure we're caught up with the newly-set low water mark. */
	if (POOL_NEEDS_CATCHUP(pp) && pool_catchup(pp) != 0) {
		/*
		 * XXX: Should we log a warning?  Should we set up a timeout
		 * to try again in a second or so?  The latter could break
		 * a caller's assumptions about interrupt protection, etc.
		 */
	}
}

void
pool_sethiwat(struct pool *pp, int n)
{

	pp->pr_maxpages = (n == 0)
		? 0
		: roundup(n, pp->pr_itemsperpage) / pp->pr_itemsperpage;
}

int
pool_sethardlimit(struct pool *pp, u_int n, const char *warnmsg, int ratecap)
{
	int error = 0;

	if (n < pp->pr_nout) {
		error = EINVAL;
		goto done;
	}

	pp->pr_hardlimit = n;
	pp->pr_hardlimit_warning = warnmsg;
	pp->pr_hardlimit_ratecap.tv_sec = ratecap;
	pp->pr_hardlimit_warning_last.tv_sec = 0;
	pp->pr_hardlimit_warning_last.tv_usec = 0;

done:
	return (error);
}


void
pool_set_constraints(struct pool *pp, const struct kmem_pa_mode *mode)
{
	pp->pr_crange = mode;
}



#if defined POOL_DEBUG || defined DDB
int
pool_chk_page(struct pool *pp, struct pool_item_header *ph, int expected)
{
	struct pool_item *pi;
	caddr_t page;
	int n;
#if defined(DIAGNOSTIC) && defined(POOL_DEBUG)
	int i, *ip;
#endif
	const char *label = pp->pr_wchan;

	page = (caddr_t)((u_long)ph & pp->pr_alloc->pa_pagemask);

    if ( (ph->ph_page + pp->pr_phoffset) != (caddr_t)ph && /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4133 */
	    (pp->pr_roflags & PR_PHINPAGE) != 0) {
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_subr_pool_009) */
		TCIP_OBSD_TRACE_02(0, OBSD_TRACE_LEVEL_ERROR, "pool(%p): page inconsistency: page %p;", pp, page);
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_subr_pool_009) */
		printf("%s: ", label);
		printf("pool(%p:%s): page inconsistency: page %p; "
		    "at page head addr %p (p %p)\n",
		    pp, pp->pr_wchan, ph->ph_page, ph, page);
		return 1;
	}

	for (pi = TAILQ_FIRST(&ph->ph_itemlist), n = 0;
	     pi != NULL;
	     pi = TAILQ_NEXT(pi,pi_list), n++) {

		page =
		    (caddr_t)((u_long)pi & pp->pr_alloc->pa_pagemask);
		if (page == ph->ph_page)
			continue;

/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_subr_pool_010) */
		TCIP_OBSD_TRACE_02(0, OBSD_TRACE_LEVEL_ERROR, "pool(%p): page inconsistency: page %p;", pp, page);
		TCIP_OBSD_TRACE_03(0, OBSD_TRACE_LEVEL_ERROR, "item ordinal %d; addr %p (p %p)", n, pi, page);
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_subr_pool_010) */
		printf("%s: ", label);
		printf("pool(%p:%s): page inconsistency: page %p;"
		    " item ordinal %d; addr %p (p %p)\n", pp,
		    pp->pr_wchan, ph->ph_page, n, pi, page);
		return 1;
	}
	if (n + ph->ph_nmissing != pp->pr_itemsperpage) {
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_subr_pool_011) */
		TCIP_OBSD_TRACE_02(0, OBSD_TRACE_LEVEL_ERROR, "pool(%p): page inconsistency: page %p;", pp, page);
		TCIP_OBSD_TRACE_03(0, OBSD_TRACE_LEVEL_ERROR, "%d on list, %d missing, %d items per page", n, ph->ph_nmissing, pp->pr_itemsperpage);
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_subr_pool_011) */
		printf("pool(%p:%s): page inconsistency: page %p;"
		    " %d on list, %d missing, %d items per page\n", pp,
		    pp->pr_wchan, ph->ph_page, n, ph->ph_nmissing,
		    pp->pr_itemsperpage);
		return 1;
	}
	if (expected >= 0 && n != expected) {
/* OBSD_TRACE_PATCH_START - patch no. (obsd_kernel_subr_pool_012) */
		TCIP_OBSD_TRACE_02(0, OBSD_TRACE_LEVEL_ERROR, "pool(%p): page inconsistency: page %p;", pp, page);
		TCIP_OBSD_TRACE_03(0, OBSD_TRACE_LEVEL_ERROR, "%d on list, %d missing, %d expected", n, ph->ph_nmissing, expected);
/* OBSD_TRACE_PATCH_END - patch no. (obsd_kernel_subr_pool_012) */
		printf("pool(%p:%s): page inconsistency: page %p;"
		    " %d on list, %d missing, %d expected\n", pp,
		    pp->pr_wchan, ph->ph_page, n, ph->ph_nmissing,
		    expected);
		return 1;
	}
	return 0;
}
int
pool_chk(struct pool *pp)
{
	struct pool_item_header *ph;
	int r = 0;

	LIST_FOREACH(ph, &pp->pr_emptypages, ph_pagelist)
		r += pool_chk_page(pp, ph, pp->pr_itemsperpage);
	LIST_FOREACH(ph, &pp->pr_fullpages, ph_pagelist)
		r += pool_chk_page(pp, ph, 0);
	LIST_FOREACH(ph, &pp->pr_partpages, ph_pagelist)
		r += pool_chk_page(pp, ph, -1);

	return (r);
}

#endif /* POOL_DEBUG ||  DDB */


/* pool_flush_pages has to be called  at appropriate spl level (typically splnet)                 */
void pool_flush_pages(struct pool *pp)
{
	struct pool_item_header *ph, *ph_temp;
	struct pool_item *pi;
	caddr_t cp;
	int n;
	/*int n2 = pp->pr_nout;*/

	/* 
	 * printf("----------------- fullpages-----------------\n");
	 * pool_print_pagelist(&pp->pr_fullpages, db_printf);
	 * printf("----------------- partpages-----------------\n");
	 * pool_print_pagelist(&pp->pr_partpages, db_printf);
	*/

	/* need to flush all none empty pages before destroying the pool */
	LIST_FOREACH_SAFE(ph, &pp->pr_fullpages, ph_pagelist, ph_temp) 
	{
		cp = ph->ph_colored;
		n = ph->ph_nmissing;

		while (n--) 
		{
			pool_put(pp, cp);
			cp += pp->pr_size;
		}
	}

	LIST_FOREACH_SAFE(ph, &pp->pr_partpages, ph_pagelist, ph_temp) 
	{
		cp = ph->ph_colored;
		n = ph->ph_nmissing;

		do 
		{
			TAILQ_FOREACH(pi, &ph->ph_itemlist, pi_list) 
			{
				if (cp == (caddr_t)pi)
					break;
			}
			if (cp != (caddr_t)pi) 
			{
				pool_put(pp, cp);
				n--;
			}

			cp += pp->pr_size;
		} while (n > 0);
	}
	/*
	 * printf("----------------- fullpages-----------------\n");
	 * pool_print_pagelist(&pp->pr_fullpages, db_printf);
	 * printf("----------------- partpages-----------------\n");
	 * pool_print_pagelist(&pp->pr_partpages, db_printf);
	*/
}

/*
 * Pool backend allocators.
 *
 * Each pool has a backend allocator that handles allocation, deallocation
 */
void	*pool_page_alloc(struct pool *, int, int *);
void	pool_page_free(struct pool *, void *);

/*
 * safe for interrupts, name preserved for compat this is the default
 * allocator
 */
struct pool_allocator pool_allocator_nointr = {
	pool_page_alloc, pool_page_free, 0,
};




/*
 * XXX - we have at least three different resources for the same allocation
 *  and each resource can be depleted. First we have the ready elements in
 *  the pool. Then we have the resource (typically a vm_map) for this
 *  allocator, then we have physical memory. Waiting for any of these can
 *  be unnecessary when any other is freed, but the kernel doesn't support
 *  sleeping on multiple addresses, so we have to fake. The caller sleeps on
 *  the pool (so that we can be awakened when an item is returned to the pool),
 *  but we set PA_WANT on the allocator. When a page is returned to
 *  the allocator and PA_WANT is set pool_allocator_free will wakeup all
 *  sleeping pools belonging to this allocator. (XXX - thundering herd).
 *  We also wake up the allocator in case someone without a pool (malloc)
 *  is sleeping waiting for this allocator.
 */

void *
pool_allocator_alloc(struct pool *pp, int flags, int *slowdown)
{
	void *v;

	v = pp->pr_alloc->pa_alloc(pp, flags, slowdown);

	return (v);
}

void
pool_allocator_free(struct pool *pp, void *v)
{
	struct pool_allocator *pa = pp->pr_alloc;

	(*pa->pa_free)(pp, v);
}




void *
pool_page_alloc(struct pool *pp, int flags, int *slowdown)
{
	OBSD_UNUSED_ARG(pp);
	OBSD_UNUSED_ARG(flags);
	OBSD_UNUSED_ARG(slowdown); /* gerlach/gh2289n: currently not used, avoid compiler warning C4100: unreferenced formal parameter */
	return (OSAL_km_alloc(PAGE_SIZE));
}

void
pool_page_free(struct pool *pp, void *v)
{
	OBSD_UNUSED_ARG(pp); /* gerlach/gh2289n: currently not used, avoid compiler warning C4100: unreferenced formal parameter */
	OSAL_km_free(v, PAGE_SIZE);
}



void *
pool_large_alloc(struct pool *pp, int flags, int *slowdown)
{
	/* splvm(); not needed, because we don't use uvm */
	void *v = OSAL_km_alloc(pp->pr_alloc->pa_pagesz);
	OBSD_UNUSED_ARG(flags);
	OBSD_UNUSED_ARG(slowdown); /* gerlach/gh2289n: currently not used, avoid compiler warning C4100: unreferenced formal parameter */
	return (v);
}

void
pool_large_free(struct pool *pp, void *v)
{
	/* splvm(); not needed, because we don't use uvm */
    OSAL_km_free(v, pp->pr_alloc->pa_pagesz);
}

void *
pool_large_alloc_ni(struct pool *pp, int flags, int *slowdown)
{
	/* splvm(); not needed, because we don't use uvm */
	OBSD_UNUSED_ARG(flags);
	OBSD_UNUSED_ARG(slowdown); /* gerlach/gh2289n: currently not used, avoid compiler warning C4100: unreferenced formal parameter */
	return OSAL_km_alloc(pp->pr_alloc->pa_pagesz);
}

void
pool_large_free_ni(struct pool *pp, void *v)
{
    OSAL_km_free(v, pp->pr_alloc->pa_pagesz);
}

#if (defined(OBSD_TCIP_INFO_INTF))

/************************************************************************************************************************************/
/* Task 2609759 - TCIP Info Interface
*
*  Establishes info interface for pool memory statistics
*
*/

/* Get first pool from pool list "pool_head" */
extern void * obsd_pool_stat_getfirst(void)
{
	return ((void *)TAILQ_FIRST(&pool_head));
}

/* Get next pool */
extern void * obsd_pool_stat_getnext(void * pp)
{
	struct pool *pp_ = (struct pool *)pp;

	return ((void *)TAILQ_NEXT(pp_, pr_poollist));
}

/* Get pool info name */
extern const char * obsd_pool_stat_chan_get(void * pp)
{
	struct pool * pp_ = (struct pool *)pp;

	return pp_->pr_wchan;
}

/* Get pool info items */
extern unsigned long obsd_pool_stat_item_get(void * pp, int item)
{
	struct pool * pp_ = (struct pool *)pp;

	switch (item)
	{
	case 1:
	case 2:
		return pp_->pr_size;
	case 3:
		return pp_->pr_nget;
	case 4:
		return pp_->pr_nfail;
	case 5:
		return pp_->pr_nput;
	case 6:
		return pp_->pr_npagealloc;
	case 7:
		return pp_->pr_npagefree;
	case 8:
		return pp_->pr_npages;
	case 9:
		return pp_->pr_hiwat;
	case 10:
		return pp_->pr_minpages;
	case 11:
		return pp_->pr_maxpages;
	case 12:
		return (pp_->pr_maxpages*pp_->pr_itemsperpage);
	case 13:
		return pp_->pr_nidle;
	case 14:
		return pp_->pr_hardlimit;
	default:
		return -2;
	}
	return -1;
}
#endif /* OBSD_TCIP_INFO_INTF */

/************************************************************************************************************************************/




