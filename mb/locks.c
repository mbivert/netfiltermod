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
#include <asm/io.h>
#include <linux/inet.h>
#include <linux/vmalloc.h>
#include <net/ip.h>
//#include <linux/mutex.h>
#include <linux/spinlock.h>

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

//static struct mutex m;
//static DEFINE_SPINLOCK(lock);

/* This is the hook function itself */
unsigned int
hook_func(unsigned int hooknum, struct sk_buff *skb,
		const struct net_device *in, const struct net_device *out,
		int (*okfn)(struct sk_buff *)) {
	unsigned long flags = 0;

	static DEFINE_SPINLOCK(foo);
	
	//mutex_lock(&m);	
	//spin_lock_irqsave(&lock, flags);
	spin_lock_irq(&foo);
	printk(KERN_CRIT "Hello, world!\n");
	spin_unlock_irq(&foo);
	//spin_unlock_irqrestore(&lock, flags);
	//mutex_unlock(&m);	
}

/* Initialisation routine */
int init_module()
{
	/* Fill in our hook structure */
	nfho.hook     = hook_func;         /* Handler function */
	nfho.hooknum  = NF_INET_FORWARD; /* NF_INET_FORWARD */
	nfho.pf       = PF_INET;
	nfho.priority = NF_IP_PRI_FIRST;   /* Make our function first */

	//mutex_init(&m);
	nf_register_hook(&nfho);

#if 0
	printk(KERN_CRIT "Hello, world!\n");
	mutex_lock(&m);	
	mutex_unlock(&m);	
#endif

	return 0;
}

/* Cleanup routine */
void cleanup_module()
{
	//nf_unregister_hook(&nfho);
}

