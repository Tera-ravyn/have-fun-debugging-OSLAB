#include "printk.h"
#include "sbi.h"

extern void test();

int start_kernel() {
    //printk("Kernel is running!");

    test(); // DO NOT DELETE !!!

	return 0;
}
