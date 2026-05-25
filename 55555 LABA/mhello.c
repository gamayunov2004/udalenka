#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Artem Gamayunov");
MODULE_DESCRIPTION("Simple hello world kernel module for lab 5");

static int __init mhello_init(void)
{
    printk(KERN_INFO "Hello, World from mhello module\n");
    return 0;
}

static void __exit mhello_exit(void)
{
    printk(KERN_INFO "Goodbye from mhello module\n");
}

module_init(mhello_init);
module_exit(mhello_exit);
