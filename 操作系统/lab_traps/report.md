# OS Lab02: Traps

赖永凡 PB20061210
22/10/23

## Part I: RISC-V assembly

### 实验简介

运行$ make fs.img。观察call.sam代码，回答相关问题，将答案写在answers-traps.txt中

### 实验内容

```c
answers-traps.txt:
  1 1. a0 - a7, a2
  2 2. The compiler computes the result of f(8) before generating the asm code.
  Therefore there is no direct call to f in main, but the value of f(8) has been 
  prepared precedingly at 0x00 and 0x0e.
  3 3. 0x630
  4 4. 0x38
  5 5. He110, World; 0x00726c64; No need.
  6 6. It will print the value stored in a2.
```

## Part II: Backtrace

### 实验简介

你需要在kernel/printf.c中完成一个<kbd>Backtrace()</kbd>函数，该函数会将在进程栈内各个函数的栈帧下保存的返回地址依次打出。之后，分别在<kbd>sys_sleep()</kbd>和<kbd>panic()</kbd>中调用该函数，并测试结果，如果和参考的输出类似则代表你完成了这一部分。

### 实验内容

1）首先在kernel/defs.h中添加<kbd>Backtrace()</kbd>的函数原型
```c
/* kernel/defs.h */
...
 79 // printf.c
 80 void            printf(char*, ...);
 81 void            panic(char*) __attribute__((noreturn));
 82 void            printfinit(void);
 83 void            backtrace(void);
...
```
2）将提示中所给的<kbd>r_fp()</kbd>函数写入kernel/riscv.h中，以便于获得调用函数栈帧的地址。
```c
/* kernel/riscv.h */
...
356 static inline uint64
357 r_fp()
358 {
359   uint64 x;
360   asm volatile("mv %0, s0" : "=r" (x) );
361   return x;
362 }
```
3）结合提示，并观察所给的讲义文件，RISC-V中进程栈向下生长。假设某一栈帧的地址为$p$，则保存的返回地址位于$p$ - 8，上一栈帧的地址保存在$p$ - 16。此外，<kbd>PGROUNDDOWN(p)</kbd>和<kbd>PGROUNDUP(p)</kbd>两个宏定义的量分别计算除了进程栈的底部地址和顶部地址。由此，借由链表的思想，我们不难写出<kbd>Backtrace()</kbd>的代码：
```c
/* kernel/printf.c */
...
137 void
138 backtrace(void)
139 {
140   uint64 frame_p = r_fp();
141
142   while(frame_p < PGROUNDUP(frame_p) && frame_p > PGROUNDDOWN(frame_p)) {
143     printf("%p\n", *(uint64*)(frame_p - 8));
144     frame_p = *(uint64*)(frame_p - 16);
145   }
146 }
```
这里我们通过判断frame_p是否在进程栈内来决定是否循环，值得一提的是，由于进程栈是向下生长的，因此理论上来说，仅判断顶部是否越界即可。但由于不知道最后一个栈帧中的frame_p - 16所存的值为多少，这里也将下界的判断加上。
4）最后，将<kbd>Backtrace()</kbd>加入<kbd>sys_sleep()</kbd>和<kbd>panic()</kbd>中：
```c
/* kernel/sysproc.c */
...
 55 uint64
 56 sys_sleep(void)
 57 {
 58   int n;
 59   uint ticks0;
 60   backtrace();
...
/* kernel/printf.c */
...
117 void
118 panic(char *s)
119 {
120   backtrace();
...
```
进入Xv6后，执行$ bttest：
```bash
$ bttest
0x0000000080002cfe
0x0000000080002bd8
0x00000000800028ac
```
回到终端中，运行$ addr2line -e kernel/kernel：
```
root@VM5018-OS:~/xv6-labs-2020# addr2line -e kernel/kernel
0x0000000080002cfe
/root/xv6-labs-2020/kernel/sysproc.c:62
0x0000000080002bd8
/root/xv6-labs-2020/kernel/syscall.c:144
0x00000000800028ac
/root/xv6-labs-2020/kernel/trap.c:76
CTRL-D
```
和实验要求中的类似，至此我们就完成了Part II。

## Part III Alarm

### 实验简介

在这个实验中，你需要给xv6系统添加一个新的功能：alarm，以便于在进程使用CPU资源时周期性地触发。 
你需要添加一个新的系统调用函数<kbd>sigalarm(interval, handler)</kbd>。如果一个进程调用了<kbd>sigalarm(n, fn)</kbd>，那么该进程每占用n "tick"的CPU时间，kernel都将会调用一次<kbd>fn()</kbd>，并且当<kbd>fn()</kbd>返回后进程将从中断处恢复运行。如果一个进程调用了<kbd>sigalarm(0, 0)</kbd>，那么kernel将会中止alarm功能。 
你可以在user目录下找到alarmtest.c文件，其中包含了三个测试。为了证明alarm功能的完整性，你需要通过alarmtest的测试，以及，为了防止你的代码影响到系统的其余功能，你还需要通过usertests的测试。

### 实验内容

简单起见，本次实验内容可划分成两个阶段，第一阶段面向**test 0**，主要关注在kernel中调用handler函数；第二阶段则面向**test 1/test 2**，主要关注从handler中返回后，原先进程的恢复。
显然，第二阶段是前一阶段的更进一步，为了更好地展现代码是如何一步步构建的，本实验报告也将就两个阶段分别撰写内容。

#### Test 0

1）将 $U/_alarmtest\ 加入到Makefile中，以便于向xv6中添加alarmtest功能。(略)
2）根据提示，在user/user.h中声明用户空间中的系统调用：
```c
/* user/user.h */
...
  4 // system calls
  5 int fork(void);
...
 26 int sigalarm(int ticks, void (*handler)());
 27 int sigreturn(void);
...
```
3）因为<kbd>sigalarm()</kbd>和<kbd>sigreturn()</kbd>属于系统调用，因此我们需要按照添加一个系统调用的步骤，完善user/usys.pl，kernel/syscall.h和kernel/syscall.c。(略)
4）根据提示，<kbd>sys_sigreturn()</kbd>函数暂时仅需直接返回0即可。
5）<kbd>sys_sigalarm()</kbd>函数需要将alarm周期和handler函数地址保存到PCB中。此外，我们还需要在PCB中追踪该进程已经走过的tick数：
```c
/* kernel/proc.h */
...
 85 // Per-process state
 86 struct proc {
...
 97   // alarm related
 98   int interval;
 99   uint64 handler;
100   int ticks_pass;
...
/* kernel/sysproc.c */
...
100 uint64
101 sys_sigreturn(void)
102 {
103   return 0;
104 }
105
106 uint64
107 sys_sigalarm(void)
108 {
109   struct proc *p = myproc();
110   int n_interval;
111   uint64 handler_addr;
112   if(argint(0, &n_interval) < 0)
113     return -1;
114   if(argaddr(1, &handler_addr) < 0)
115     return -1;
116   p->interval = n_interval;
117   p->handler = handler_addr;
118   p->ticks_pass = 0;
119   return 0;
120 }
```
6）我们可以在proc.c中的<kbd>allocproc()</kbd>函数中对ticks_pass进行初始化。
```c
/* kernel/proc.c */
...
 93 allocproc(void)
 94 {
 95   struct proc *p;
...
130   // Initialize ticks to 0
131   p->ticks_pass = 0;
132
133   return p;
134 }
...
```
7）每个tick，硬件时钟电路都会触发中断，调用kernel/trap.c中的<kbd>usertrap()</kbd>，因此我们需要在该函数中执行ticks_pass++。但值得一提的是，并非只有间隔1 tick时才会引发中断，因此我们需要先判断是否此次中断是由时钟电路触发的。
8）如果alarm的计数器到期后，就需要进入handler函数，而这是一个用户空间的函数。观察现有程序，不难发现<kbd>usertrap()</kbd>返回后，接下去执行的指令地址储存在p->trapframe->epc中，因此我们只需将handler的地址赋给该寄存器即可。
```c
/* kernel/trap.c */
...
 36 void
 37 usertrap(void)
 38 {
 39   int which_dev = 0;
...
 79   // give up the CPU if this is a timer interrupt.
 80   if(which_dev == 2) {
 81     yield();
 82
 83     p->ticks_pass++;
 84     // alarm time out
 85     if(p->ticks_pass == p->interval)
 86       p->trapframe->epc = p->handler;
 87   }
...
```
9）至此，我们完成了第一阶段的所有任务。即，我们的程序已经可以在alarm计数器到期后转入handler函数执行。进入xv6，执行$ alarmtest的话，test 0可以通过。
```bash
$ alarmtest
test0 start
........alarm!
test0 passed
test1 start
.................................................QEMU: Terminated
```

#### test1/test2

1）为了从handler函数中返回，我们需要事先保存进程的trapframe到PCB中。在实验设计的过程中，handler函数最后都会执行<kbd>sigreturn()</kbd>系统调用来恢复到原先进程。因此，我们要在<kbd>sys_sigreturn()</kbd>中还原进程的trapframe。

2）为了防止在handler函数中重新调用handler函数，我们需要一个标志来区别一般进程与handler模式。此外，当进程调用<kbd>sigalarm(0, 0)</kbd>时，我们同样需要关闭alarm功能。对于以上功能涉及的变量，我们在初始化时也应考虑在内。

```c
/* kernel/proc.h */
...
 97   // alarm related
 98   int interval;
 99   uint64 handler;
100   int ticks_pass;
101   struct trapframe *alarm_frame;
102   int handler_flag;
...
/* kernel/proc.c */
...
 93 allocproc(void)
 94 {
 95   struct proc *p;
...
130   // Initialize ticks to 0
131   p->ticks_pass = 0;
132   p->handler_flag = 0;
133   p->interval = 0;
134
135   return p;
136 }
/* kernel/trap.c */
...
 79   // give up the CPU if this is a timer interrupt.
 80   if(which_dev == 2) {
 81     yield();
 82     // when it's not in handler and alarm is enable
 83     if (!p->handler_flag && p->interval) {
 84       p->ticks_pass++;
 85       // alarm time out
 86       if(p->ticks_pass == p->interval) {
 87         p->alarm_frame = (struct trapframe*)kalloc();
 88         memmove(p->alarm_frame, p->trapframe, sizeof(struct 
 trapframe)); // save the trapframe
 89         p->trapframe->epc = p->handler;
 90         p->handler_flag = 1; // enter handler mode
 91         p->ticks_pass = 0;
 92       }
 93     }
 94   }
...
 /* kernel/sysproc.c */
...
100 uint64
101 sys_sigreturn(void)
102 {
103   struct proc *p = myproc();
104   memmove(p->trapframe, p->alarm_frame, sizeof(struct trapframe));
105   p->handler_flag = 0; // exit handler mode
106   return 0;
107 }
...
```
## 实验结果
```
== Test answers-traps.txt == answers-traps.txt: OK 
== Test backtrace test == 
$ make qemu-gdb
backtrace test: OK (4.6s) 
== Test running alarmtest == 
$ make qemu-gdb
(5.9s) 
== Test   alarmtest: test0 == 
  alarmtest: test0: OK 
== Test   alarmtest: test1 == 
  alarmtest: test1: OK 
== Test   alarmtest: test2 == 
  alarmtest: test2: OK 
== Test usertests == 
$ make qemu-gdb
usertests: OK (276.3s) 
== Test time == 
time: OK 
Score: 85/85
```

## 实验总结

第二次做xv6的实验，相关操作都熟悉了不少，对实验环境以及系统调用的过程也有了更深入的理解。实验乍看上去没多少头绪，但其实只要沉下心来，跟着Hint一步一步做，最后也能慢慢理解具体的实现思路。 
此外，经过这次实验，我复习了“指针”、“地址”、“内存”及相关操作，也从“tick”的角度切入，多了解了一些OS的基本特性。 
