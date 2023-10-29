# OS lab03: Multithreading

赖永凡 PB20061210
2022/11/5

## Part I: Uthread

### 实验简介

在这个实验中，你需要设计并实现一个用户层面的context切换机制。uthread.c 包含大部分用户层面用到的线程相关的包和一个包含三条简单线程的测试代码。然而，所给的包中有关进程建立和切换的函数中缺失了部分代码，因此你的工作便是来完善这些代码。

### 实验内容

实验开始，运行xv6并执行$ uthread指令，没有任何输出。这很正常因为我们还没有完成和进程切换相关的代码。

1）根据说明，需要往user/uthread.c中的<kbd>thread_schedule()</kbd>函数，以及user/uthread_switch.S中的<kbd>thread_switch</kbd>函数添加代码，从而实现context切换功能。<kbd>thread_switch</kbd>函数用于寄存器的保存与恢复，而<kbd>thread_schedule()</kbd>则通过调用<kbd>thread_switch</kbd>来实现进程间的切换。此外，提示强烈建议我们通过修改thread结构体来储存寄存器。

不难联想到，kernel中也有进程切换的相关函数，我们便希望能够借助他山之石来完成任务。转到kernel中查看相关代码后发现，kernel/proc.h中最开始便定义了一个context结构体，其中包含ra（保存函数返回地址的寄存器）、sp（栈指针）以及s0到s11这12个 *callee-saved* 寄存器，此外kernel/swtch.S中也提供了context的切换汇编函数代码，其思想便是以传入的寄存器为基址，通过寄存器偏移来寻址后续寄存器。函数具体功能便是先保存当前进程的context，再导入下一进程的context。

所谓线程切换和进程切换其实并无差别，因此我们直接将kernel中的代码复制粘贴到uthread.c中即可。值得一提的是，因为uthread.c是用户空间中的函数，因此我们需要重新定义一遍context结构体。

```c
/* user/uthread.c */
...
 13 // Saved registers for thread context switches.
 14 struct context {
 15   uint64 ra;
 16   uint64 sp;
 17
 18   // callee-saved
 19   uint64 s0;
 20   uint64 s1;
 21   uint64 s2;
 22   uint64 s3;
 23   uint64 s4;
 24   uint64 s5;
 25   uint64 s6;
 26   uint64 s7;
 27   uint64 s8;
 28   uint64 s9;
 29   uint64 s10;
 30   uint64 s11;
 31 };
 32
 33 struct thread {
 34   char       stack[STACK_SIZE]; /* the thread's stack */
 35   int        state;             /* FREE, RUNNING, RUNNABLE */
 36   struct context thread_context;
 37
 38 };
...
 55 void
 56 thread_schedule(void)
 57 {
...
 82     // YOUR CODE HERE
 83     // Invoke thread_switch to switch from t to next_thread:
 84     // thread_switch(??, ??);
 85     
 86     thread_switch((uint64)&t->thread_context, (uint64)&current_thread->thread_context);
...

/* user/uthread_switch.S */
...
  8   .globl thread_switch
  9 thread_switch:
 10   /* YOUR CODE HERE */
 11   sd ra, 0(a0)
 12   sd sp, 8(a0)
 13   sd s0, 16(a0)
 14   sd s1, 24(a0)
 15   sd s2, 32(a0)
 16   sd s3, 40(a0)
 17   sd s4, 48(a0)
 18   sd s5, 56(a0)
 19   sd s6, 64(a0)
 20   sd s7, 72(a0)
 21   sd s8, 80(a0)
 22   sd s9, 88(a0)
 23   sd s10, 96(a0)
 24   sd s11, 104(a0)
 25
 26   ld ra, 0(a1)
 27   ld sp, 8(a1)
 28   ld s0, 16(a1)
 29   ld s1, 24(a1)
 30   ld s2, 32(a1)
 31   ld s3, 40(a1)
 32   ld s4, 48(a1)
 33   ld s5, 56(a1)
 34   ld s6, 64(a1)
 35   ld s7, 72(a1)
 36   ld s8, 80(a1)
 37   ld s9, 88(a1)
 38   ld s10, 96(a1)
 39   ld s11, 104(a1)
 40   ret    /* return to ra */
```

2）在确定了context的内容后，<kbd>thread_create()</kbd>中的初始化就比较直观了：将传入的函数指针存入ra中，以及初始化栈指针sp，值得注意的是，这里的栈是向下生长的。

```c
/* user/uthread.c */
...
 91 void
 92 thread_create(void (*func)())
 93 {
 94   struct thread *t;
 95
 96   for (t = all_thread; t < all_thread + MAX_THREAD; t++) {
 97     if (t->state == FREE) break;
 98   }
 99   t->state = RUNNABLE;
100   // YOUR CODE HERE
101   t->thread_context.ra = (uint64)func;
102   t->thread_context.sp = (uint64)(t->stack + STACK_SIZE);
103 }
...
```
至此，我们就完成了第一部分的所有任务。再次运行xv6，执行$ uthread，可以看到以下输出：
```bash
$ uthread
thread_a started
thread_b started
thread_c started
thread_c 0
thread_a 0
thread_b 0
...
thread_c 99
thread_a 99
thread_b 99
thread_c: exit after 100
thread_a: exit after 100
thread_b: exit after 100
thread_schedule: no runnable threads
```

## Part II: Using threads

### 实验简介

在这个实验中，你将会借由一个哈希表的例子，探索LINUX系统中的多线程以及互斥技术。
notxv6/ph.c中包含了一个简单的哈希表。如果仅使用一个线程来读写这张表的话，当前的代码并没有任何问题。输入以下指令后，可以看到类似的输出：（ph传入的参数表示线程的数量）
```bash
$ make ph
$ ./ph 1
100000 puts, 12.595 seconds, 7940 puts/second
0: 0 keys missing
100000 gets, 12.557 seconds, 7964 gets/second
```
但是，当使用两个线程同时对哈希表进行读写时，程序就会发生错误：
```bash
$ ./ph 2
100000 puts, 5.869 seconds, 17040 puts/second
1: 16622 keys missing
0: 16622 keys missing
200000 gets, 13.809 seconds, 14483 gets/second
```
ph包含两个模块，其中先通过调用<kbd>put()</kbd>函数来实现对哈希表键值对的添加，之后再通过<kbd>get()</kbd>函数来对哈希表进行查找。那些被添加但却无法在表中查找到（即在线程间的并发工作中丢失）的表项将会被统计为missing。此外，程序还会计算出每秒get和put的平均项数。 
通过上面的输出不难看出，当两个线程并发工作时，的确实现了2倍的效率。然而，由于缺少必要措施，线程的并发存在着严重的问题。为应对这一问题，显然你需要用到互斥环境。
因此，实验任务即为解决这一并发问题，并且使得最终的工作速度不低于单线程速度的1.25倍。

### 实验内容

1）如果只是想解决并发中的错误问题，其实不需要太多的分析，直接在所有<kbd>put()</kbd>和<kbd>get()</kbd>语句的前后加上lock和unlock语句即可。诚然，这样可以完美地清零misssing，但这样效率甚至不如单线程（存在线程间切换的开销），因此我们需要重新设计互斥环境。
```bash
$ ./ph 2
100000 puts, 12.921 seconds, 7739 puts/second
1: 0 keys missing
0: 0 keys missing
200000 gets, 25.874 seconds, 7730 gets/second
```

2）在这之前，我们需要先回答为何会出现missing的问题，以便于我们对症下药：
在课堂中我们学到，当两个程序同时修改临界区的时候就会不可避免地产生错误。当然，这里的临界区就是这张哈希表，但其实并非所有关于临界区的读写操作都需要互斥的环境。显然所有的读操作就都是可以并行的，因此<kbd>get()</kbd>语句的前后的lock和unlock都是非必要的。 
那么<kbd>put()</kbd>语句又如何呢？如果单纯地一次只能让一个进程进行写操作，那么效率将永远无法超过单线程，因此我们需要深入put操作，而不是简单地将整个函数锁住。观察<kbd>put()</kbd>函数，不难发现其中唯一涉及到对哈希表进行修改的只有insert操作。（由于这里两进程所写的key的集合不相交，所以修改表项值操作不存在并发的问题） 
我们继续看insert函数，发现这是一个针对哈希表每一个桶的头插法插入操作。换言之，实验中的哈希表实际上就是5条链表，使用时计算 *key%5* 来进行散列。而插入时需要传入所散列到的链表的表头，之后将新建的表项作为新的表头，储存回哈希表中。**因此，如果线程1正在对某条链表进行插入操作但尚未更新表头。这时线程2同时对同一链表进行插入，就会得到线程1插入之前的表头。虽然线程1离开后会将表头更新为自己先前插入的那一项，但之后线程2离开时也会再次更新表头，这样线程1插入的表项就丢失了。**

3）在明白missing产生的机制之后，我们可以知道，真正的临界区其实是哈希表中的每一条链表（一共5个），而不同链表之间是可以并行修改的。因此，我们可以对每一个临界区都设一把锁，从而在确保正确的前提下提升效率：
```c
/* ph.c */
...
 20 // declare a bunch of locks
 21 pthread_mutex_t lock0;
 22 pthread_mutex_t lock1;
 23 pthread_mutex_t lock2;
 24 pthread_mutex_t lock3;
 25 pthread_mutex_t lock4;
...
 45 static
 46 void put(int key, int value)
 47 {
 48   int i = key % NBUCKET;
 49
 50   // is the key already present?
 51   struct entry *e = 0;
 52   for (e = table[i]; e != 0; e = e->next) {
 53     if (e->key == key)
 54       break;
 55   }
 56   if(e){
 57     // update the existing key.
 58     e->value = value;
 59   } else {
 60     // the new is new.
 61     switch (i)
 62     {
 63     case 0: {
 64       pthread_mutex_lock(&lock0);
 65       insert(key, value, &table[i], table[i]);
 66       pthread_mutex_unlock(&lock0);
 67     } break;
...
 83     case 4: {
 84       pthread_mutex_lock(&lock4);
 85       insert(key, value, &table[i], table[i]);
 86       pthread_mutex_unlock(&lock4);
 87     } break;
 88     }
 89
 90   }
 91 }
...
134 int
135 main(int argc, char *argv[])
136 {
...
140
141   // initialize the lock
142   pthread_mutex_init(&lock0, NULL);
143   pthread_mutex_init(&lock1, NULL);
144   pthread_mutex_init(&lock2, NULL);
145   pthread_mutex_init(&lock3, NULL);
146   pthread_mutex_init(&lock4, NULL);
...
```
再次运行程序，得到了预期的结果：
```bash
$ ./ph 2
100000 puts, 6.379 seconds, 15676 puts/second
1: 0 keys missing
0: 0 keys missing
200000 gets, 12.373 seconds, 16164 gets/second
```

## Part III: Barrier

### 实验简介

在这个实验中，你需要完成一个barrier，即在所有程序都到达barrier之前，先到达的程序将会被卡在barrier处。 
文件notxv6/barrier.c中包含了一个“破损”的barrier，同样，该程序传入的参数代表着线程的个数，其中每个线程中都包含了一个循环体，只有当所有线程都到达<kbd>barrier()</kbd>处后，方可开始下一轮的循环。
你的实验任务便是让程序达成预期的效果，除了在上个程序中用到的互斥相关的原语之外，你可能还需要用到同步相关的原语：
```c
pthread_cond_wait(&cond, &mutex); // go to sleep on cond, releasing lock mutex,
// acquiring upon wake up
pthread_cond_broadcast(&cond); // wake up every thread sleeping on cond
```
值得注意的是，调用<kbd>pthread_cond_broadcast(&cond, &mutex)</kbd>会释放mutex，但是当程序被唤醒后又会重新回到mutex中。

### 实验内容

实验中<kbd>barrier_init()</kbd>已经写好，因此我们只需要在<kbd>barrier()</kbd>中添加代码。
除了实现barrier的功能之外，还有两点需要注意：

- 结构体bstate中记录了当前循环的轮数，你需要在每次barrier释放时增加轮数。

- 需要避免出现barrier中阻碍的进程还未全部离开，先离开的进程又再次进入barrier中，从而导致barrier的进程计数出现错误。

```c
/* barrier.c */
...
 25 static void
 26 barrier()
 27 {
 28   // YOUR CODE HERE
 29   //
 30   // Block until all threads have called barrier() and
 31   // then increment bstate.round.
 32   //
 33   pthread_mutex_lock(&bstate.barrier_mutex);
 34   bstate.nthread++;
 35   if(bstate.nthread == nthread) {
 36     pthread_cond_broadcast(&bstate.barrier_cond);
 37     bstate.nthread = 0;
 38     bstate.round++;
 39     pthread_mutex_unlock(&bstate.barrier_mutex);
 40   }
 41   else {
 42     pthread_cond_wait(&bstate.barrier_cond, &bstate.barrier_mutex);
 43     // For the waken thread still holds mutex
 44     // We need to release it first
 45     pthread_mutex_unlock(&bstate.barrier_mutex);
 46   }
 47 }
...
```
由于<kbd>pthread_cond_broadcast(&bstate.barrier_cond)</kbd>一次性可以释放出所有blocked的程序，因此我们只需要在最后一个线程到来时调用一次即可，由于此时仍在mutex环境中，故让该线程顺便更新round和清零nthread，在这之后才释放mutex。 
这里我们将整个barrier视为临界区，因此进入时需要mutex环境。至于mutex的释放，分两种情况，一种是线程被blocked时主动退出，另一种则是上文中所述的情况。
此外，注意到第45行的unlock语句，由于被唤醒的程序又会回到mutex中，故这里需要重新释放mutex环境。

## 实验结果

```
== Test uthread ==
$ make qemu-gdb
uthread: OK (8.0s)
== Test answers-thread.txt == answers-thread.txt: OK
== Test ph_safe == make[1]: 进入目录“/root/xv6-labs-2020”
make[1]: “ph”已是最新。
make[1]: 离开目录“/root/xv6-labs-2020”
ph_safe: OK (18.4s)
== Test ph_fast == make[1]: 进入目录“/root/xv6-labs-2020”
make[1]: “ph”已是最新。
make[1]: 离开目录“/root/xv6-labs-2020”
ph_fast: OK (43.8s)
== Test barrier == make[1]: 进入目录“/root/xv6-labs-2020”
make[1]: “barrier”已是最新。
make[1]: 离开目录“/root/xv6-labs-2020”
barrier: OK (2.7s)
== Test time ==
time: OK
Score: 60/60
```

## 实验思考题

**1. Uthread: switching between threads:thread_switch needs to save/restore only the callee-save registers. Why?** 

在调用汇编函数 <kbd>uthread_switch()</kbd> 的过程中，caller-saved registers 已经被调用者保存到栈帧中了，所以这里无需保存这一部分寄存器。

**2. Using threads: Why are there missing keys with 2 threads, but not with 1 thread? Identify a sequence of events with 2 threads that can lead to a key being missing.**

如果线程1正在对某条链表进行插入操作但尚未更新表头。这时线程2同时对同一链表进行插入，就会得到线程1插入之前的表头。虽然线程1离开后会将表头更新为自己先前插入的那一项，但之后线程2离开时也会再次更新表头，这样线程1插入的表项就丢失了。显然只有一个进程是不存在这样的问题。

## 实验感想

本次实验主题是多线程，其中用到了课内学到的互斥与同步的技术，让我印象深刻。一是切身体会到了并发的问题并非是纸上谈兵，而是切实存在的隐患；二是由于在课堂中学习过了相关的知识（包括barrier），因此实验做起来比较得心应手。
