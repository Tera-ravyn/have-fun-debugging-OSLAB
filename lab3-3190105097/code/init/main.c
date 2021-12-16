#include "printk.h"
#include "sbi.h"

extern void test();

int start_kernel() {
    printk("Hello RISC-V!\nidle process is running!\n");

    test(); // DO NOT DELETE !!!

	return 0;
}
