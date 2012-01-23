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
#include <linux/init.h>
#include <linux/skbuff.h>
#include <linux/tcp.h>
#
#include <asm/io.h>
#include <linux/inet.h>
#include <linux/vmalloc.h>
#include <net/ip.h>

#include <linux/netdevice.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,16)
#include <linux/in.h>
#include <linux/ip.h>
#endif

MODULE_AUTHOR("Afnarel"); 
MODULE_LICENSE("GPL GPLv3"); 

/* This is the structure we shall use to register our function */
static struct nf_hook_ops nfho_forward;
static struct nf_hook_ops nfho_localout;

static int nb_created = 0;

/* Name of the interface we want to drop packets from */
struct sk_buff *sock_buff;
struct iphdr *ip_header;
struct tcphdr *tcp_header;

/* This is the hook function itself */
unsigned int hook_func(unsigned int hooknum, struct sk_buff *skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *)) {
	const struct net_device_stats *stats_in;
	const struct net_device_stats *stats_out;

	// Ok, useless but ...
	sock_buff = skb;
	
	if(!sock_buff){ return NF_ACCEPT; }
	ip_header = (struct iphdr *) skb_network_header (sock_buff);
	if(!(ip_header)){ return NF_ACCEPT; }
	if(ip_header->protocol != 6 ){ return NF_ACCEPT; }
	tcp_header = (struct tcphdr *) skb_transport_header (sock_buff);
	if (!tcp_header) { return NF_ACCEPT; }
	
	// printk(KERN_CRIT "Packets received: %lu, Packets transmitted: %lu \n", in->stats.rx_packets, in->stats.tx_packets);
	// printk(KERN_CRIT "Rx Errors: %lu, Tx Errors: %lu \n", in->stats.rx_errors, in->stats.tx_errors);
	// printk(KERN_CRIT "Rx Dropped: %lu, Tx Dropped: %lu \n", in->stats.rx_dropped, in->stats.tx_dropped);

	//printk(KERN_CRIT "In device name : %s // Out device name : %s \n", in->name, out->name);

	stats_in = dev_get_stats((struct net_device*)in);
	stats_out = dev_get_stats((struct net_device*)out);
	printk(KERN_CRIT "IN : Received : %lu // Transmitted : %lu\n", stats_in->rx_packets, stats_in->tx_packets);
	printk(KERN_CRIT "OUT : Received : %lu // Transmitted : %lu\n", stats_out->rx_packets, stats_out->tx_packets);
	printk(KERN_CRIT "Queue 1 : %lu\n", stats_out->tx_packets - stats_in->rx_packets);
	printk(KERN_CRIT "Queue 2 : %lu\n", stats_in->tx_packets - stats_out->rx_packets);
	printk(KERN_CRIT "Nb created : %d\n", nb_created);

/*
	printk(KERN_CRIT "%6s:%8lu %7lu %4lu %4lu %4lu %5lu %10lu %9lu "
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
*/
	
	//printk(KERN_CRIT "In device name : %s // Out device name : %s \n", in->name, out->name);
	//printk(KERN_CRIT "Avec SKBUFF : %s \n", skb->nf_bridge->physindev->name);
	
	return NF_ACCEPT;
}

/* This is the hook function itself */
unsigned int hook_func2(unsigned int hooknum, struct sk_buff *skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *)) {

	// Ok, useless but ...
	sock_buff = skb;
	
	if(!sock_buff){ return NF_ACCEPT; }
	ip_header = (struct iphdr *) skb_network_header (sock_buff);
	if(!(ip_header)){ return NF_ACCEPT; }
	if(ip_header->protocol != 6 ){ return NF_ACCEPT; }
	tcp_header = (struct tcphdr *) skb_transport_header (sock_buff);
	if (!tcp_header) { return NF_ACCEPT; }
	
	nb_created++;
	//printk(KERN_CRIT "Nb created : %d\n", nb_created);
	
	return NF_ACCEPT;
}

/* Initialisation routine */
int init_module() {
	/* Fill in our hook structure */
	nfho_forward.hook     = hook_func;         /* Handler function */
	nfho_forward.hooknum  = NF_INET_FORWARD; /* NF_IP_FORWARD */ // NF_INET_POST_ROUTING
	nfho_forward.pf       = PF_INET;
	nfho_forward.priority = NF_IP_PRI_FIRST;   /* Make our function first */

	/* Fill in our hook structure */
	nfho_localout.hook     = hook_func2;         /* Handler function */
	nfho_localout.hooknum  = NF_INET_LOCAL_OUT; /* NF_IP_FORWARD */ // NF_INET_POST_ROUTING
	nfho_localout.pf       = PF_INET;
	nfho_localout.priority = NF_IP_PRI_FIRST;   /* Make our function first */

	printk(KERN_CRIT "Go ahead\n");
	nf_register_hook(&nfho_forward);
	nf_register_hook(&nfho_localout);

	return 0;
}
/* Cleanup routine */
void cleanup_module() {
	nf_unregister_hook(&nfho_forward);
	nf_unregister_hook(&nfho_localout);
}