## Lab 3

#### 4.1 准备工程

![image-20211125144113722](/home/himeros/Asche/OS/lab3-3190105097/image-20211125144113722.png)

调整了文件结构后出现了如图错误，排查后发现是因为`include`文件夹中加入了新的文件，但是在lab1中`lib`文件夹里自己写的makefile比较原始低级，没有自动检查并添加新文件进行编译链接的功能，修改了该makefile文件后能正常编译运行。

#### 4.2 `proc.h`数据结构定义

好像没有啥需要自己写的

#### 4.3 线程调度功能实现

##### 4.3.1 线程初始化

```c
//arch/riscv/kernel/proc.c

extern void __dummy();

struct task_struct* idle;           // idle process
struct task_struct* current;        // 指向当前运行线程的 `task_struct`
struct task_struct* task[NR_TASKS]; // 线程数组，所有的线程都保存在此

void task_init() {
    int i=0;
    idle = (struct task_struct*)kalloc();// 1. 调用 kalloc() 为 idle 分配一个物理页
    idle->state = TASK_RUNNING;// 2. 设置 state 为 TASK_RUNNING
    idle->counter = 0;
    idle->priority = 0;// 3. 由于 idle 不参与调度 可以将其 counter / priority 设置为 0
    idle->pid = 0;// 4. 设置 idle 的 pid 为 0
    current = idle;
    task[0] = idle;// 5. 将 current 和 task[0] 指向 idle

     for(i = 1; i < NR_TASKS; i++) { // 1. 参考 idle 的设置, 为 task[1] ~ task[NR_TASKS - 1] 进行初始化
        task[i] = (struct task_struct*)kalloc(); // 2. 其中每个线程的 state 为 TASK_RUNNING, counter 为 0, priority 使用 rand() 来设置, pid 为该线程在线程数组中的下标。
        task[i]->state = TASK_RUNNING;
        task[i]->counter = 0;
        task[i]->priority = rand();
        task[i]->pid = i;
        task[i]->thread.ra = (uint64)__dummy;   // 3. 为 task[1] ~ task[NR_TASKS - 1] 设置 `thread_struct` 中的 `ra` 和 `sp`,
        task[i]->thread.sp = (uint64)(task[i] + 4096);    // 4. 其中 `ra` 设置为 __dummy （见 4.3.2）的地址， `sp` 设置为 该线程申请的物理页的高地址
    }

    printk("...proc_init done!\n");
}

```

![image-20211202155203575](/home/himeros/Asche/OS/lab3-3190105097/image-20211202155203575.png)

卡住了Σ(   °Д °;)，然后发现task struct的指针还啥都没指呢，改了改现在能正常初始化跑起来了

![image-20211202164208910](/home/himeros/Asche/OS/lab3-3190105097/image-20211202164208910.png)

##### 4.3.2 __dummy和dummy

```s
    .global __dummy
__dummy:

	la t0, dummy
        csrw sepc, t0
        sret
```



一直卡在__dummy跳转dummy上。

![image-20211202223735606](/home/himeros/Asche/OS/lab3-3190105097/image-20211202223735606.png)![image-20211202223839524](/home/himeros/Asche/OS/lab3-3190105097/image-20211202223839524.png)

之后就卡死

##### 4.3.3 实现线程切换

```c
//switch_to

void switch_to(struct task_struct* next) {
    if (current->pid != next->pid) {//判断下一个要执行的线程是否与当前线程相同
         printk("ready to switch\n");
         __switch_to(current, next);
         current = next;
         printk("switch to [PID = %d COUNTER = %d]\n", current->pid, current->counter); 
    }
}
```

```s
#__switch_to

    .global __switch_to
__switch_to:
        sd ra,40(a0) #a0中存的是task_struct指针prev，储存其寄存器的信息
        sd sp,48(a0)
        sd s0,56(a0)
        sd s1,64(a0)
        sd s2,72(a0)
        sd s3,80(a0)
        sd s4,88(a0)
        sd s5,96(a0)
        sd s6,104(a0)
        sd s7,112(a0)
        sd s8,120(a0)
        sd s9,128(a0)
        sd s10,136(a0)
        sd s11,144(a0)
        ld ra,40(a1) #a1中存的是task_struct指针next，读取其寄存器的信息
        ld sp,48(a1)
        ld s0,56(a1)
        ld s1,64(a1)
        ld s2,72(a1)
        ld s3,80(a1)
        ld s4,88(a1)
        ld s5,96(a1)
        ld s6,104(a1)
        ld s7,112(a1)
        ld s8,120(a1)
        ld s9,128(a1)
        ld s10,136(a1)
        ld s11,144(a1)
        ret
```

##### 4.3.4 实现调度入口函数

```c
void do_timer(void) {
    if(current->pid == task[0]->pid) schedule(); //1. 如果当前线程是 idle 线程 直接进行调度
    else {
        current->counter--; // 2. 如果当前线程不是 idle 对当前线程的运行剩余时间减1 
        printk("SET [PID = %d COUNTER = %d]\n", current->pid, current->counter);
        if (current->counter > 0) return;//3.若剩余时间仍然大于0 则直接返回 否则进行调度
        else schedule();
    }
}
```

##### 4.3.5 实现线程调度

###### 4.3.5.1 短作业优先调度算法

###### 4.3.5.2 优先级调度算法

![image-20211206013012869](/home/himeros/Asche/OS/lab3-3190105097/image-20211206013012869.png)

![image-20211216213804606](/home/himeros/Asche/OS/lab3-3190105097/image-20211216213804606.png)

![image-20211216213823605](/home/himeros/Asche/OS/lab3-3190105097/image-20211216213823605.png)![image-20211216213907676](/home/himeros/Asche/OS/lab3-3190105097/image-20211216213907676.png)
