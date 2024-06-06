#include <linux/module.h>
#include <linux/init.h>


static int __init embedi_init(void)
{
	printk(KERN_INFO "hello embedi\n");
	return 0;
}

static void __exit embedi_exit(void)
{
	printk(KERN_INFO "goodbye embedi\n");
}

module_init(embedi_init);
module_exit(embedi_exit);
MODULE_LICENSE("GPL");
