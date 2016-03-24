#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/sched.h>

asmlinkage int sys_status(void) {
	printk("state: %ld\n", current->state);
	printk("pid: %d\n", current->pid);
	printk("prio: %d, static_prio: %d, normal_prio: %d\n", current->prio, current->static_prio, normal_prio);
	printk("fpu_counter: %d\n", current->fpu_counter);
	printk("policy: %d\n", current->policy);
	printk("exit_state: %d, exit_code: %d, exit_signal %d\n", current->exit_state, current->exit_code, current->exit_signal);
	printk("pdeath_signal: %d\n", current->pdeath_signal);
	printk("jobctl: %d\n", current->jobctl);
	printk("utime: %d, stime: %d\n", current->utime, current->stime);
	printk("prev_utime: %d, prev_stime: %d\n", current->prev_utime, current->prev_stime);
	printk("(context switch counts)nvcsw: %ld, nivcsw: %ld", current->nvcsw, current->nivcsw);
	return 0;
}
