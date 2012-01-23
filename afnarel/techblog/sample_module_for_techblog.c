#include <linux/module.h> 
MODULE_AUTHOR("PG Aasis Vinayak"); 
MODULE_LICENSE("GPL GPLv3"); 
static int start__module(void) {
	printk("This is a sample module for techblog!\n"); 
	return 0; 
} 
static void end__module(void) { 
	printk("From techblog.aasisvinayak.com!\n"); 
} 
module_init(start__module); 
module_exit(end__module);