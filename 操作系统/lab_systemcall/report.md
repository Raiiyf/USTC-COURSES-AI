# OS Lab01: System call

赖永凡 PB20061210
22/10/16

## Part I: System call tracing (moderate)

### 实验简介

在这个部分中，你需要添加一个新的功能*trace*来追踪xv6的系统调用。为了实现这个功能，首先要新增一个系统调用函数<kbd>sys_trace</kbd>。

该功能的输入格式为：
```
trace mask other_command
```
用户空间的 trace.c 中的函数可以理解成使能了这个系统调用追踪功能，从而OS会在执行other_command时，根据mask的值来输出对应的系统调用的信息。

```bash
$ trace 32 grep hello README
3: syscall read -> 1023
3: syscall read -> 966
3: syscall read -> 70
3: syscall read -> 0
```
```C
/* kernel/syscall.h */
1 // System call numbers 
2 #define SYS_fork    1
3 #define SYS_exit    2
4 #define SYS_wait    3
5 #define SYS_pipe    4
6 #define SYS_read    5
```
在上面这个例子中，$32 = 2^5$ 为1左移5（即系统调用read的标号）位，因此OS就会开始追踪指令：*grep hello README* 中使用到的read的输出情况。

P.S. 用户空间的 trace.c 中的函数已经完成了，因此剩下需要完成的功能只有mask的追踪和系统调用信息的打印。

### 实验内容

1）往Makefile中的UPROGS子模块添加<kbd>$U/_trace</kbd>
```makefile
135 UPROGS=\
136   $U/_cat\
...
148   $U/_sysinfotest\
149   $U/_trace\
...
```
这里是为了在编译的时候将用户空间里的trace.c文件囊括进入。
2）为了能够成功编译，我们还需要在用户空间中声明系统调用函数：
```h
/* user/user.h */
...
 26 int uptime(void);
 27 int trace(int); 
 28 int sysinfo(struct sysinfo*);
```
添加接口函数
```pl
# user/usys.pl 
...
 38 entry("uptime");
 39 entry("trace");
 40 entry("sysinfo");
```
为我们添加的新系统调用函数（sys_trace）添加标号
```h
/* kernel/syscall.h */
...
 21 #define SYS_mkdir  20
 22 #define SYS_close  21
 23 #define SYS_trace  22
 24 #define SYS_sysinfo 23
```
这样一来，我们就完成了添加一个系统调用的前期准备工作，接下去就可以着手具体功能的实现了。

3）往<kbd>kernel/sysproc.c</kbd>中添加<kbd>sys_trace()</kbd>。结合先前的分析和实验手册上的提示，我们需要往PCB中新增一项trace_mask，而系统调用<kbd>sys_trace()</kbd>的功能即是将其读入的mask参数写到调用进程的PCB中，以便于持续追踪该进程的系统调用。
```h
/* kernel/proc.h */
...
 86 struct proc {
 87   struct spinlock lock;
 88
 89   // p->lock must be held when using these:
 90   enum procstate state;        // Process state
...
 95   int pid;                     // Process ID
 96   int trace_mask;              // Trace Mask
 97
 98   // these are private to the process, so p->lock need not be held.
 99   uint64 kstack;               // Virtual address of kernel 
...
107 };

```
通过观察已经完成的程序，argint(0, &n)函数可以将系统调用的int型参数存入变量n中，而myproc()函数可以获得唤起OS的进程的PCB。因此不难完成<kbd>sys_trace()</kbd>的功能：
```c
/* kernel/sysproc.c */
...
100 // trace the mask
101 uint64
102 sys_trace(void)
103 {
104   int n;
105   if(argint(0, &n) < 0)
106     return -1;
107   myproc()->trace_mask = n;
108   return 0;
109 }
```

4）由于trace在进程中的追踪需要被进程fork()出的子程序继承，因此我们需要对fork()稍作修改，使得trace_mask能够被继承。
```c
/* kernel/proc.c */
...
258 int
259 fork(void)
260 {
...
292   // pass the trace mask
293   np->trace_mask = p->trace_mask;
...
303   return pid;
304 }
```

5）现在到了最后一步，根据提示，我们需要修改<kbd>syscall()</kbd>来输出trace的结果。
```c
/* kernel/syscall.c */
...
142 void
143 syscall(void)
144 {
145   int num;
146   struct proc *p = myproc();
147
148   num = p->trapframe->a7;//system call num
149   if(num > 0 && num < NELEM(syscalls) && syscalls[num]) {
150     p->trapframe->a0 = syscalls[num]();//system call return num
151     if((p->trace_mask & (1 << num)) != 0) {
152       printf("%d: syscall %s -> %d\n", p->pid, syscall_names[num], 
p->trapframe->a0);
153     }
154   } else {
155     printf("%d %s: unknown sys call %d\n",
156             p->pid, p->name, num);
157     p->trapframe->a0 = -1;
158   }
159 }
```
为了方便说明，这里先给出最终的代码。Syscall()可以看做是连接进程和具体的系统调用的接口函数，它首先从进程获取系统调用号（a7），然后根据标号来决定进入哪一个系统调用，执行结束后再将返回值存回寄存器a0。因此，我们只需在系统调用执行结束后判断是否要将本次调用信息输出即可，而这个问题的答案便储存在进程PCB的trace_mask中。

根据实验简介中的说明，mask对应二进制数下某一位（bit[31:0]）若为1，则以该位为标号的进程就需要被追踪。因此我们可以通过位运算的方式，判断trace_mask的
bit[i]（i为刚才执行的系统调用号）是否为1即可。

实验要求我们输出进程号，系统调用名称，系统调用返回值。对于进程号和返回值，我们都可以在PCB中轻松获得，而对于系统调用的名称，我们还需要手动添加一个以标号为下标，对应系统名称字符串为元素的字符数组了。
```C
/* kernel/syscall.c */
...
136 static char *syscall_names[24] = {
137 "", "fork", "exit", "wait", "pipe", "read", "kill", "exec", "fstat",
138 "chdir", "dup", "getpid", "sbrk", "sleep", "uptime", "open", "write",
139 "mknod", "unlink", "link", "mkdir", "close", "trace", "sysinfo"
140 };
```
由于第一个系统调用fork()的标号是从1开始的，因此需要在最开始添加一个空字符串。

## Part II: Sysinfo (moderate)

### 实验简介

在这个部分中，你需要添加一个新的系统调用sysinfo，该函数传入并返回一个结构体指针<kbd>struct sysinfo</kbd>
```C
// kernel/sysinfo.h
  1 struct sysinfo {
  2   uint64 freemem;   // amount of free memory (bytes)
  3   uint64 nproc;     // number of process
  4 };
```
其中 freemem 项储存系统中的可用内存的字节数，nproc 项储存系统中进程中状态不为<kbd>UNUSED</kbd>的进程数。实验中已经写好了一个测试函数<kbd>sysinfotest</kbd>，若该程序输出 "sysinfotest: OK" 则代表着你完成了这个实验。

### 实验内容

1）往Makefile中的UPROGS子模块添加<kbd>$U/_sysinfotest</kbd>。（和trace部分相同，不再列出）
2）做添加新系统调用的准备工作，这里的步骤大部分和trace部分相同，不再赘述（在trace部分的代码中也能够看到sysinfo的身影）。不同的是，因为我们在kernel中新定义了结构体变量，所以需要在用户空间中声明。
```c
/* user/user.h */
  1 struct stat;
  2 struct rtcdate;
  3 struct sysinfo;
```
3）为了获得系统中的可用空间字节数，我们需要在kernel/kalloc.c中新建一个函数，这里我命名为<kbd>kfreemem()</kbd>。通过观察kalloc.c中的代码，我们可以发现两点：
* xv6系统内存管理的最小单位是PAGE，其对应的字节数为PGSIZE，系统分配、回收内存都是以PAGE为单位进行的。
* OS会维护一个结构体kmem，其下的链表freelist便储存着系统中所有可用PAGE的指针

知道这两点后，为了统计出可用空间字节数，我们只需要计算一遍链表的长度，然后再乘以PGSIZE即可。值得一提的是，观察其他代码，在对kmem进行读操作时需要维护一个互斥环境。
```c
/* kernel/kalloc.c */
...
 84 uint64
 85 kfreemem(void)
 86 {
 87   struct run *r;
 88
 89   uint64 n = 0;
 90   acquire(&kmem.lock);
 91   r = kmem.freelist;
 92   while(r)
 93   {
 94     n++;
 95     r = r->next;
 96   }
 97   release(&kmem.lock);
 98
 99   return n * PGSIZE;
100 }
```

4）为了统计出系统中状态不为<kbd>UNUSED</kbd>的进程数，我们需要在kernel/proc.c中新建一个函数，这里我命名为<kbd>nproc()</kbd>。通过观察proc.c的代码，发现了一个<kbd>allocproc()</kbd>函数：
```c
/* kernel/proc.c */
...
 92 static struct proc*
 93 allocproc(void)
 94 {
 95   struct proc *p;
 96
 97   for(p = proc; p < &proc[NPROC]; p++) {
 98     acquire(&p->lock);
 99     if(p->state == UNUSED) {
100       goto found;
101     } else {
102       release(&p->lock);
103     }
104   }
105   return 0;
106
107 found:
...
```
该函数通过检索proc表来获得系统中进程PCB的指针，受到它的启发，我们不难写出以下的代码：（同样需要互斥环境）
```c
/* kernel/proc.c */
...
700 //count num of processes whose state is not UNUSED
701 uint64
702 nproc(void)
703 {
704   uint64 n = 0;
705   struct proc *p;
706   for(p = proc; p < &proc[NPROC]; p++) {
707     acquire(&p->lock);
708     if(p->state != UNUSED)
709       n++;
710     release(&p->lock);
711   }
712   return n;
713 }
```

5）最后一步，我们需要完成<kbd>sys_sysinfo()</kbd>函数（我添加在了kernel/sysproc.c中）来将获得到的信息传回用户空间的调用进程中。
```c
/* kernel/sysproc.c */
...
114 uint64
115 sys_sysinfo(void)
116 {
117   struct sysinfo system_info;
118   system_info.freemem = kfreemem();
119   system_info.nproc = nproc();
120
121   uint64 arg_p;
122   struct proc *p = myproc();
123   // fetch the address passes from user space process
124   if(argaddr(0, &arg_p) < 0)
125     return -1;
126   if(copyout(p->pagetable, arg_p, (char *)&system_info, 
sizeof(system_info)) < 0)
127     return -1;
128   return 0;
129 }
```

具体的实现思路类似之前的<kbd>sys_trace()</kbd>，先通过 argaddr(0, &user_p) 获得进程传入的地址，然后再根据提示模仿已有代码，利用<kbd>copyout()</kbd>函数将结构体sysinfo储存在该地址中。

至此，我们就完成了这个实验所需要的全部步骤。

## 实验结果

本次实验花时大概5小时，在xv6的目录下新建time.txt，写入5。

运行<kbd>$ make grade</kbd>，得到以下结果：

```bash
== Test trace 32 grep ==
$ make qemu-gdb
trace 32 grep: OK (5.6s)
== Test trace all grep ==
$ make qemu-gdb
trace all grep: OK (1.9s)
== Test trace nothing ==
$ make qemu-gdb
trace nothing: OK (1.8s)
== Test trace children ==
$ make qemu-gdb
trace children: OK (24.1s)
== Test sysinfotest ==
$ make qemu-gdb
sysinfotest: OK (6.1s)
== Test time ==
time: OK
Score: 35/35
```

## 实验感想

本次是第一次自己上手来做OS实验，最大的问题还是在于实验环境的不够熟悉（包括xv6中诸多的文件、代码内容、C语言编程以及vim的使用）。完成第一部分（trace）时，我只是简单跟着提示，在指定位置添加指定的功能。虽然最后也完成了实验目标，但对于背后的原因尚不甚理解。

在开始第二部分后，由于提示的变少，我开始感到无从下手，故开始比较前后的实验步骤，试图理解xv6系统调用的背后逻辑（整理在最后）。此外，还有一个问题就是忽视了这是在一个操作系统上进行的功能实现。例如在编写<kbd>kfreemem()</kbd>函数时，其实不难看出系统在kmem中维护了关于可用内存指针的链表。但我一开始却想要先调用某个函数计算一下kmem结构体，再对其进行遍历，而忽视了kmem结构体是由OS自动维护的，只需在互斥状态下直接读取即可。

不过初次实验收获也颇丰，最关键的是课堂上学到的内容在实验中得到了验证。此外，根据提示、现有代码以及实验结果来对整个OS的工作状态不断摸索的过程也很令人着迷（只可惜ddl太紧，只能浅尝辄止）。

---

最后附上我总结的OS系统调用的过程：

假设我们要使用一个系统调用xxx，我们会遇到这些量：
```c
xxx(); // 用户空间中的系统调用函数
sys_xxx(); // 核空间中的系统调用函数
SYS_xxx; // 核空间中系统调用函数的标号
```
当你在用户空间想使用xxx系统调用时，你就使用xxx()函数。虽然我们没有写过任何关于xxx()的具体定义，但在user/usys.pl中，我们已经添加了xxx的路口，所以当检测到你使用了"xxx"字段的函数后，就会将SYS_xxx（xxx的标号）保存到a7寄存器中，然后通过ecall唤醒OS。
OS同样从a7中读取你使用的系统调用号，然后根据kernel/syscall.c中的一个函数数组（可以理解成数组的元素是函数），来执行sys_xxx()函数。执行完毕后再将返回值存入a0寄存器中，返回用户空间、还原现场。至此系统调用便执行结束，之后原进程继续运行。
```c
// user/usys.pl
  9 sub entry {
 10     my $name = shift;
 11     print ".global $name\n";
 12     print "${name}:\n";
 13     print " li a7, SYS_${name}\n";
 14     print " ecall\n";
 15     print " ret\n";
 16 }
 17 
 18 entry("fork");
 19 entry("exit");
...

// kernel/syscall.c
110 static uint64 (*syscalls[])(void) = {
111 [SYS_fork]    sys_fork,
112 [SYS_exit]    sys_exit,
...
134 };
```
于是，如果我们要新增一个系统调用xxx，那么我们需要：
```c
// 首先是kernel中的准备

// syscall.h中添加xxx的标识码
#define SYS_sysinfo 23
#define SYS_xxx 24

// 在sysproc.c（与进程相关）或sysfile.c（与文件相关）内
// 添加 uint64 sys_xxx(void) 函数，写入你想实现的功能
uint 64
sys_xxx(void) {
  /* 你想要的功能 */
}

// 之后我们需要在syscall.c中声明sys_xxx()
extern uint64 sys_sysinfo(void);
// 并在函数路口表中添加我们新增的系统调用
static uint64 (*syscalls[])(void) = {
...
[SYS_xxx] sys_xxx,
};
// 为了使xxx能够被trace到，我们还需要把它加入到系统调用名称表中
static char *syscall_names[25] = {
"", "fork", "exit", "wait", "pipe", "read", "kill", "exec", "fstat",
"chdir", "dup", "getpid", "sbrk", "sleep", "uptime", "open", "write",
"mknod", "unlink", "link", "mkdir", "close", "trace", "sysinfo",
"xxx"
};

// 之后是user中的准备

// 这里我们需要在user.h中加入xxx的声明
// system calls
...
int xxx(***); //***表示你想要的参数类型

// 最后我们需要在usys.pl中添加连接user和kernel的路口
entry("xxx");

// 至此，就完整地添加了一个新的系统调用函数
```
