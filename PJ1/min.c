#include <linux/kernel.h>
#include <linux/linkage.h>

asmlinkage long min(long a, long b) {
	return (a < b)? a: b;
}
