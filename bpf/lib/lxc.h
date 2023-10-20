/* SPDX-License-Identifier: (GPL-2.0-only OR BSD-2-Clause) */
/* Copyright Authors of Cilium */

#ifndef __LIB_LXC_H_
#define __LIB_LXC_H_

#include "common.h"
#include "utils.h"
#include "ipv6.h"
#include "ipv4.h"
#include "eth.h"
#include "dbg.h"
#include "trace.h"
#include "csum.h"
#include "l4.h"
#include "proxy.h"
#include "proxy_hairpin.h"

#define TEMPLATE_LXC_ID 0xffff

#ifdef ENABLE_SIP_VERIFICATION
static __always_inline
int is_valid_lxc_src_ip(struct ipv6hdr *ip6 __maybe_unused)
{
#ifdef ENABLE_IPV6
	union v6addr valid = {};

	BPF_V6(valid, LXC_IP);

	return ipv6_addr_equals((union v6addr *)&ip6->saddr, &valid);
#else
	return 0;
#endif
}

static __always_inline
int is_valid_lxc_src_ipv4(const struct iphdr *ip4 __maybe_unused)
{
#ifdef ENABLE_IPV4
	return ip4->saddr == LXC_IPV4;
#else
	/* Can't send IPv4 if no IPv4 address is configured */
	return 0;
#endif
}
#else /* ENABLE_SIP_VERIFICATION */
static __always_inline
int is_valid_lxc_src_ip(struct ipv6hdr *ip6 __maybe_unused)
{
	return 1;
}

static __always_inline
int is_valid_lxc_src_ipv4(struct iphdr *ip4 __maybe_unused)
{
	return 1;
}
#endif /* ENABLE_SIP_VERIFICATION */

#ifdef ENABLE_SMAC_VERIFICATION
static __always_inline
int is_valid_lxc_src_mac(struct __ctx_buff *ctx)
{
	union macaddr lxc_mac = LXC_MAC;
	void *data = ctx_data(ctx), *data_end = ctx_data_end(ctx);
	struct ethhdr *eth = data;
	union macaddr *smac = NULL;
	if (data + 12 > data_end)
		return 1;
	smac = (union macaddr *) &eth->h_source;
	return !eth_addrcmp(smac, &lxc_mac);
}
#else /* ENABLE_SMAC_VERIFICATION */
static __always_inline
int is_valid_lxc_src_mac(struct __ctx_buff *ctx __maybe_unused) {
	return 1;
}
#endif /* ENABLE_SMAC_VERIFICATION */
#endif /* __LIB_LXC_H_ */
