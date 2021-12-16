#include "printk.h"
#include "defs.h"

// Please do not modify
unsigned long cnt = 100000000;

void test() {
    while (1) {
	
	cnt--;
	if (cnt==0) {
    		printk("Kernel is running!\n");
		cnt = 100000000;
	}
    }
}
