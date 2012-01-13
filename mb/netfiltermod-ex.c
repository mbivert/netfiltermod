/* Sample code to install a Netfilter hook function that will
 * drop all incoming packets on an interface we specify */

#ifndef __KERNEL__
        #define __KERNEL__
#endif
#ifndef MODULE
        #define MODULE
#endif

#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netdevice.h>
#include <linux/init.h>
#include <linux/skbuff.h>
#include <linux/tcp.h>
#include <asm/io.h>
#include <linux/inet.h>
#include <linux/vmalloc.h>
#include <net/ip.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,16)
#include <linux/in.h>
#include <linux/ip.h>
#endif

struct {
	int npacket;
	int speed;
	int nflow;
	int qsize;
} table[] = {
	{ 0, 200, 2, 0 },
	{ 0, 200, 2, 0 }
};

/* This is the structure we shall use to register our function */
static struct nf_hook_ops nfho;

/* Name of the interface we want to drop packets from */
struct sk_buff *sock_buff;
struct iphdr *ip_header;
struct tcphdr *tcp_header;

#define HASH(s) ((*((s)+strlen(s)-1))-'0'-1)

void
printtable(void)
{
	int i;

	for (i = 0; i < sizeof(table)/sizeof(table[0]); i++) {
		printk(KERN_CRIT "\teth%d: %d, %d, %d, %d\n", i+1, table[i].npacket,
			table[i].speed, table[i].nflow, table[i].qsize);
	}
}

/* This is the hook function itself */
unsigned int
hook_func(unsigned int hooknum, struct sk_buff *skb,
		const struct net_device *in, const struct net_device *out,
		int (*okfn)(struct sk_buff *)) {

	const struct net_device_stats *stats;

	// Ok, useless but ...
	sock_buff = skb;
	if (!sock_buff)
		return NF_ACCEPT;

	ip_header = (struct iphdr *) skb_network_header (sock_buff);
	if (!ip_header)
		return NF_ACCEPT;
	if (ip_header->protocol != 6 )
		return NF_ACCEPT;

	tcp_header = (struct tcphdr *) skb_transport_header (sock_buff);
	if (!tcp_header)
		return NF_ACCEPT;
	
	printk(KERN_CRIT "From %s to %s\n", in->name, out->name);

	/* table[HASH(in->name)].npacket++;
	table[HASH(out->name)].npacket--; */

	stats = dev_get_stats((struct net_device *)in);

	printk(KERN_CRIT  "%6s:%8lu %7lu %4lu %4lu %4lu %5lu %10lu %9lu "
		"%8lu %7lu %4lu %4lu %4lu %5lu %7lu %10lu\n",
		in->name, stats->rx_bytes, stats->rx_packets,
		stats->rx_errors,
		stats->rx_dropped + stats->rx_missed_errors,
		stats->rx_fifo_errors,
		stats->rx_length_errors + stats->rx_over_errors +
		stats->rx_crc_errors + stats->rx_frame_errors,
		stats->rx_compressed, stats->multicast,
		stats->tx_bytes, stats->tx_packets,
		stats->tx_errors, stats->tx_dropped,
		stats->tx_fifo_errors, stats->collisions,
		stats->tx_carrier_errors +
		stats->tx_aborted_errors +
		stats->tx_window_errors +
		stats->tx_heartbeat_errors,
		stats->tx_compressed);

	printtable();

	return NF_ACCEPT;
}

/* Initialisation routine */
int init_module()
{
	/* Fill in our hook structure */
	nfho.hook     = hook_func;         /* Handler function */
	nfho.hooknum  = NF_INET_FORWARD; /* NF_INET_FORWARD */
	nfho.pf       = PF_INET;
	nfho.priority = NF_IP_PRI_FIRST;   /* Make our function first */

	printk(KERN_CRIT "Go ahead\n");
	nf_register_hook(&nfho);

	return 0;
}

/* Cleanup routine */
void cleanup_module()
{
	nf_unregister_hook(&nfho);
}

