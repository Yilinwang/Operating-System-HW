#include <linux/kernel.h>
#include <linux/linkage.h>

asmlinkage int sys_show(void) {
	printk("B03902024 Hsiao-Hua Cheng, B03902084 Yi-Lin Wang!!\n");
	return 0;
}
