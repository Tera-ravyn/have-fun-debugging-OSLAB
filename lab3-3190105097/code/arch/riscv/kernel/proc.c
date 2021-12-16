#include "proc.h"
#include "rand.h"
#include "printk.h"
#include "mm.h"


extern void __dummy();
extern void __switch_to(struct task_struct* prev, struct task_struct* next);


struct task_struct* idle;           // idle process
struct task_struct* current;        // 指向当前运行线程的 `task_struct`
struct task_struct* task[NR_TASKS]; // 线程数组，所有的线程都保存在此

void task_init() {
    int i = 1;

    idle = (struct task_struct*)kalloc();
    idle->state = TASK_RUNNING;
    idle->counter = 0;
    idle->priority = 0;
    idle->pid = 0;
    current = idle;
    task[0] = idle;
    
    for(i = 1; i < NR_TASKS; i++) {
        task[i] = (struct task_struct*)kalloc(); 
        task[i]->state = TASK_RUNNING;
        task[i]->counter = 0;
        task[i]->priority = rand();
        task[i]->pid = i;
        task[i]->thread.ra = (uint64)__dummy;
        task[i]->thread.sp = (uint64)task[i] + 4096;  
    }
    printk("...proc_init done!\n");
}

void dummy() {
    uint64 MOD = 1000000007;
    uint64 auto_inc_local_var = 0;
    int last_counter = -1;
    while(1) {
        if (last_counter == -1 || current->counter != last_counter) {
            last_counter = current->counter;
            auto_inc_local_var = (auto_inc_local_var + 1) % MOD;
            printk("[PID = %d] is running. auto_inc_local_var = %d\n", current->pid, auto_inc_local_var);
        }
    }
}

void switch_to(struct task_struct* next) {
    //printk("switch_to is running, current = [%d]\n", current->pid);
    if (current->pid != next->pid) {
         printk("\nswitch to [PID = %d COUNTER = %d PRIORITY = %d]\n", next->pid, next->counter, next->priority);
         struct task_struct *prev = current;
         current = next;
         __switch_to(prev, next);
    }
}

void do_timer(void) {
    //printk("do_timer is running, current = [%d]\n", current->pid);
    if(current->pid == idle->pid) schedule();
    else {
        current->counter--;
        //printk("SET [PID = %d COUNTER = %d]\n", current->pid, current->counter);
        if (current->counter == 0) schedule();
    }
}

//SJF
#ifdef SJF
void schedule(void) {

    int i = 1;
    int min = 0;
    int flag[NR_TASKS - 1];
    flag[0] = 1;
    for (; i < NR_TASKS; i++) {
        if (task[i]->state == TASK_RUNNING) {
            if (task[i]->counter == 0) flag[i] = 1;
            else flag[i] = 0;
        }
    }
    for (i = 1; i < NR_TASKS; i++) {
        flag[0] = flag[0] * flag[i];
    }
    if (flag[0] == 1) {
        printk("\n");
        for (i = 1; i < NR_TASKS; i++) {
            task[i]->counter = rand();
            printk("SET [PID = %d COUNTER = %d]\n", task[i]->pid, task[i]->counter);
        }
        min = 1;
        for (i = 1; i < NR_TASKS; i++) {
            if (task[i]->state == TASK_RUNNING) {
                if (task[i]->counter < task[min]->counter) min = i;
            }
        }
    } else {
        for (i = 1; i < NR_TASKS; i++) {
            if (task[i]->counter != 0) break;
        }
        min = i;
        for (i = 1; i < NR_TASKS; i++) {
             if (task[i]->counter < task[min]->counter && task[i]->counter > 0) min = i;
        }
    }
    switch_to(task[min]);
}
//PRI
#else
void schedule(void) {
 /*   int i, next, c;
    while (1) {
        c = -1;
        next = 0;
        i = NR_TASKS;
        while (--i) {
            if (!task[i]) continue;
            if (task[i]->state == TASK_RUNNING && task[i]->counter > c) {
                c = task[i]->counter;
                next = i;
            }
        }
        if (c) break;
        for(i = NR_TASKS - 1; i > 0; i--) {
            if(task[i])
	        task[i]->counter = (task[i]->counter >> 1) + task[i]->priority;
        }
    }
    switch_to(task[next]);*/
       int i = 1, flag = 0;
        for(; i < NR_TASKS; i++) {
            if(task[i]->counter != 0) flag++;
        }
        if(flag == 0){
            printk("\n");
            for(i = 1; i < NR_TASKS; i++) {
                task[i]->counter = rand();
                printk("SET [PID = %d COUNTER = %d PRIORITY = %d]\n", task[i]->pid, task[i]->counter, task[i]->priority);
            }
            schedule();
        }
        i = 1;
        while(task[i]->state != TASK_RUNNING || task[i]->counter == 0){
            i++;
        }
        int pri = i;
        for(; i < NR_TASKS; i++){
            if(task[i]->priority >= task[pri]->priority && task[i]->state == TASK_RUNNING && task[i]->counter > 0){
                if(task[i]->priority > task[pri]->priority) pri = i;
                else if(task[i]->counter < task[pri]->counter) pri = i;
            }
        }
        switch_to(task[pri]);
}
#endif

