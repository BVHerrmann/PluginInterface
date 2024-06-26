/*	$OpenBSD: radix.h,v 1.17 2011/07/22 13:05:29 henning Exp $	*/
/*	$NetBSD: radix.h,v 1.8 1996/02/13 22:00:37 christos Exp $	*/

/*
 * Copyright (c) 1988, 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)radix.h	8.2 (Berkeley) 10/31/94
 */

#ifndef _NET_RADIX_H_
#define	_NET_RADIX_H_

/*
 * Radix search tree node layout.
 */

struct radix_node {
	struct	radix_mask *rn_mklist;	/* list of masks contained in subtree */
	struct	radix_node *rn_p;	/* parent */
	short	rn_b;			/* bit offset; -1-index(netmask) */
	char	rn_bmask;		/* node: mask for bit test*/
	u_char	rn_flags;		/* enumerated next */
#define RNF_NORMAL	1		/* leaf contains normal route */
#define RNF_ROOT	2		/* leaf is root leaf for tree */
#define RNF_ACTIVE	4		/* This node is alive (for rtfree) */
	union {
		struct {			/* leaf only data: */
			caddr_t	rn_Key;		/* object of search */
			caddr_t	rn_Mask;	/* netmask, if present */
			struct	radix_node *rn_Dupedkey;
		} rn_leaf;
		struct {			/* node only data: */
			int	rn_Off;		/* where to start compare */
			struct	radix_node *rn_L;/* progeny */
			struct	radix_node *rn_R;/* progeny */
		} rn_node;
	} rn_u;
};

#define rn_dupedkey rn_u.rn_leaf.rn_Dupedkey
#define rn_key rn_u.rn_leaf.rn_Key
#define rn_mask rn_u.rn_leaf.rn_Mask
#define rn_off rn_u.rn_node.rn_Off
#define rn_l rn_u.rn_node.rn_L
#define rn_r rn_u.rn_node.rn_R

/*
 * Annotations to tree concerning potential routes applying to subtrees.
 */

extern struct radix_mask {
	short	rm_b;			/* bit offset; -1-index(netmask) */
	char	rm_unused;		/* cf. rn_bmask */
	u_char	rm_flags;		/* cf. rn_flags */
	struct	radix_mask *rm_mklist;	/* more masks to try */
	union	{
		caddr_t	rmu_mask;		/* the mask */
		struct	radix_node *rmu_leaf;	/* for normal routes */
	}	rm_rmu;
	int	rm_refs;		/* # of references to this struct */
} *rn_mkfreelist;

#define rm_mask rm_rmu.rmu_mask
#define rm_leaf rm_rmu.rmu_leaf		/* extra field would make 32 bytes */

#define MKGet(m) do {							\
	if (rn_mkfreelist) {						\
		m = rn_mkfreelist;					\
		rn_mkfreelist = (m)->rm_mklist;				\
	} else								\
		R_Malloc(m, struct radix_mask *, sizeof (*(m)));	\
} while (0)

#define MKFree(m) do {							\
	(m)->rm_mklist = rn_mkfreelist;					\
	rn_mkfreelist = (m);						\
} while (0)

struct radix_node_head {
	struct	radix_node *rnh_treetop;
	int	rnh_addrsize;		/* permit, but not require fixed keys */
	int	rnh_pktsize;		/* permit, but not require fixed keys */
					/* add based on sockaddr */
	struct	radix_node *(*rnh_addaddr)(void *v, void *mask,
		     struct radix_node_head *head, struct radix_node nodes[],
		     u_int8_t prio);
					/* remove based on sockaddr */
	struct	radix_node *(*rnh_deladdr)(void *v, void *mask,
		    struct radix_node_head *head, struct radix_node *rn);
					/* locate based on sockaddr */
	struct	radix_node *(*rnh_matchaddr)(void *v,
		    struct radix_node_head *head);
					/* locate based on sockaddr */
	struct	radix_node *(*rnh_lookup)(void *v, void *mask,
		    struct radix_node_head *head);
					/* traverse tree */
	int	(*rnh_walktree)(struct radix_node_head *,
		     int (*)(struct radix_node *, void *, u_int), void *);
	struct	radix_node rnh_nodes[3];/* empty tree for common case */
	int	rnh_multipath;		/* multipath? */
	u_int	rnh_rtableid;
};

#ifdef _KERNEL
#define Bcmp(a, b, n) bcmp(((caddr_t)(a)), ((caddr_t)(b)), (unsigned)(n))
#define Bcopy(a, b, n) bcopy(((caddr_t)(a)), ((caddr_t)(b)), (unsigned)(n))
#define Bzero(p, n) bzero((caddr_t)(p), (unsigned)(n));
#define R_Malloc(p, t, n) (p = (t) malloc((unsigned long)(n), M_RTABLE, M_DONTWAIT))
#define Free(p) free((caddr_t)p, M_RTABLE);

void	rn_init(void);
int	rn_inithead(void **, int);
int	rn_inithead0(struct radix_node_head *, int);
int	rn_refines(void *, void *);
int	rn_walktree(struct radix_node_head *,
	    int (*)(struct radix_node *, void *, u_int), void *);

extern void rn_deinit(void);
extern int  rn_deinithead(void **head, int off);

struct radix_node	*rn_addmask(void *, int, int);
struct radix_node	*rn_addroute(void *, void *, struct radix_node_head *,
			    struct radix_node [2], u_int8_t);
struct radix_node	*rn_delete(void *, void *, struct radix_node_head *,
			    struct radix_node *);
struct radix_node	*rn_insert(void *, struct radix_node_head *, int *,
			    struct radix_node [2]);
struct radix_node	*rn_lookup(void *, void *, struct radix_node_head *);
struct radix_node	*rn_match(void *, struct radix_node_head *);
struct radix_node	*rn_newpair(void *, int, struct radix_node[2]);
struct radix_node	*rn_search(void *, struct radix_node *);
struct radix_node	*rn_search_m(void *, struct radix_node *, void *);
#endif /* _KERNEL */
#endif /* _NET_RADIX_H_ */
