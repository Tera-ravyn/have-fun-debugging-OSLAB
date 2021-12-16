#include "sbi.h"
#include "clock.h"

unsigned long TIMECLOCK = 10000000;

unsigned long get_cycles() {
    unsigned long time = 0;
        asm volatile (
        "rdtime %[time]\n"
        : [time] "=r" (time)
        : 
        : "memory"
    );
    return time;

}

void clock_set_next_event() {
    unsigned long next = get_cycles() + TIMECLOCK;

    sbi_ecall(0x0, 0x0, next, 0, 0, 0, 0, 0);
} 
