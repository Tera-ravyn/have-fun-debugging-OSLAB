#include "printk.h"
#include "clock.h"

void trap_handler(unsigned long scause,unsigned long sepc){
    if(scause >> 63 == 1 && (scause << 1) == 10){

            printk("[S] Supervisor Mode Timer Interrupt\n");
            clock_set_next_event();

    }
}
