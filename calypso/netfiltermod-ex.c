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
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/init.h>
#include <linux/skbuff.h>
#include <linux/tcp.h>
#include <asm/io.h>
#include <linux/inet.h>
#include <linux/vmalloc.h>
#include <net/ip.h>

#include <linux/spinlock.h>

#include <linux/interrupt.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,16)
#include <linux/in.h>
#include <linux/ip.h>
#endif

#define NB_INTFS 3
#define NB_STATS 4

/* Args */
static int outputRate_eth1 = 1000; 
static int outputRate_eth2 = 10; 
module_param(outputRate_eth1, int, 0);
module_param(outputRate_eth2, int, 0);
static int nbPackets = 0;

static DEFINE_SPINLOCK(lock); 

static int stats_packets[NB_INTFS][NB_STATS] = { { 0 } };

/* This is the structure we shall use to register our function */
static struct nf_hook_ops nfho_post;

/* Name of the interface we want to drop packets from */
struct sk_buff *sock_buff;
struct iphdr *ip_header;
struct tcphdr *tcp_header;

inline int hash_index(const char* name_intf) { 
	return name_intf[strlen(name_intf) - 1] - '0'; 
}

/* Hook function: POST */
unsigned int hook_func_post(unsigned int hooknum, struct sk_buff *skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *)) {

	int index;
	sock_buff = skb; // Ok, useless but ...
	
	if (!sock_buff) { 
		return NF_ACCEPT; }
	ip_header = (struct iphdr *) skb_network_header (sock_buff);
	if (!ip_header) { 
		return NF_ACCEPT; 
	}
	if (ip_header->protocol != 6) { 
		return NF_ACCEPT; 
	}
	tcp_header = (struct tcphdr *) skb_transport_header (sock_buff);
	if (!tcp_header) { 
		return NF_ACCEPT; 
	}
	
	printk(KERN_CRIT "POST");
	printk(KERN_CRIT "In: %s | Out: %s", in->name, out->name);

	index = hash_index(out->name);

	spin_lock_irq(&lock);
	stats_packets[index][1]++; // O++
	stats_packets[index][2] = stats_packets[index][0] - stats_packets[index][1]; // qsize = I - O
	printk(KERN_CRIT "InputRate: %d\n", stats_packets[index][0]);
	printk(KERN_CRIT "Qsize: %d\n", stats_packets[index][2]);
	spin_unlock_irq(&lock);

	return NF_ACCEPT;
}

/* Handler */
irqreturn_t handler(int irq, void* dev_id, struct pt_regs * regs){

	nbPackets++;
	printk(KERN_CRIT "NbPackets: %d\n", nbPackets);

	/*int index = 42;

	// Ok, useless but ...
	sock_buff = skb;
	
	if (!sock_buff) { 
		return IRQ_HANDLED; 
	}
	ip_header = (struct iphdr *) skb_network_header (sock_buff);
	if (!(ip_header)) { 
		return IRQ_HANDLED; 
	}
	if (ip_header->protocol != 6) { 
		return IRQ_HANDLED; 
	}
	tcp_header = (struct tcphdr *) skb_transport_header (sock_buff);
	if (!tcp_header) { 
		return IRQ_HANDLED; 
	}
	
	printk(KERN_CRIT "\nPRE");
	printk(KERN_CRIT "In: %s | Out: %s", in->name, out->name);

	if (ip_header->daddr == 29935816) { // Target: 200.200.200.1 via eth2
		index = hash_index("eth2");
	} else if (ip_header->daddr == 23356516) { // Target: 100.100.100.1 via eth1
		index = hash_index("eth1");
	}
	if (index == 42) {
		return IRQ_HANDLED;
	}
	
	spin_lock_irq(&lock);
	stats_packets[index][0]++;
	stats_packets[index][2] = stats_packets[index][0] - stats_packets[index][1]; // qsize = I - O
	printk(KERN_CRIT "InputRate: %d\n", stats_packets[index][0]);
	printk(KERN_CRIT "Qsize: %d\n", stats_packets[index][2]);
	spin_unlock_irq(&lock);*/
	
	return IRQ_HANDLED;
}

/* Initialisation routine */
int init_module()
{
	int ret = 23;

	/* Fill in our hook structure */
	nfho_post.hook     = hook_func_post;         /* Handler function */
	nfho_post.hooknum  = NF_INET_POST_ROUTING; /* NF_IP_FORWARD */
	nfho_post.pf       = PF_INET;
	nfho_post.priority = NF_IP_PRI_FIRST;   /* Make our function first */

	printk(KERN_CRIT "Go ahead\n");
	nf_register_hook(&nfho_post);

	// Interruption management
	free_irq(NET_RX_SOFTIRQ, NULL);
	ret = request_irq(NET_RX_SOFTIRQ, (irq_handler_t)handler, IRQF_SHARED, "handler paquets", (void*)(handler));

	printk(KERN_CRIT "ret = %d\n", ret);

	//stats_packets[1][1] = outputRate_eth1;
	//stats_packets[2][1] = outputRate_eth2;

	return 0;
}
/* Cleanup routine */
void cleanup_module()
{
	printk(KERN_CRIT "NbPackets: %d\n", nbPackets);
	nf_unregister_hook(&nfho_post);
	free_irq(NET_RX_SOFTIRQ, NULL);
}

MODULE_LICENSE("GPL");
