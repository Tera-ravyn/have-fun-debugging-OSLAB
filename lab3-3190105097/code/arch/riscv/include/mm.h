#include "types.h"

struct run {
    struct run *next;//我有点看不懂
};

void mm_init();

uint64 kalloc();
void kfree(uint64);
