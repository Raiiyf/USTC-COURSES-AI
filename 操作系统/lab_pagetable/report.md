# OS Lab04: Page tables

赖永凡 PB20061210
22/11/20

## Part I: Print a page table

### 实验简介

为了更好地了解RISC-V的页表以及在后续任务中更好地debug，第一个任务将是完成一个函数来打印页表内容。

定义一个函数<kbd>vmprint()</kbd>，函数的传入参数类型为pagetable_t，同时函数能按照以下的格式打印页表内容。
在exec.c中return argc一行前面插入该行代码：if(p->pid==1) vmprint(p->pagetable); 从而xv6将会在初始化后输出其第一个进程的页表内容。

```bash
page table 0x0000000087f6e000
..0: pte 0x0000000021fda801 pa 0x0000000087f6a000
.. ..0: pte 0x0000000021fda401 pa 0x0000000087f69000
.. .. ..0: pte 0x0000000021fdac1f pa 0x0000000087f6b000
.. .. ..1: pte 0x0000000021fda00f pa 0x0000000087f68000
.. .. ..2: pte 0x0000000021fd9c1f pa 0x0000000087f67000
..255: pte 0x0000000021fdb401 pa 0x0000000087f6d000
.. ..511: pte 0x0000000021fdb001 pa 0x0000000087f6c000
.. .. ..510: pte 0x0000000021fdd807 pa 0x0000000087f76000
.. .. ..511: pte 0x0000000020001c0b pa 0x0000000080007000
```

其中，第一行输出函数传入的参数，在这之后，每一行对应着一个PTE（包含作为页表指针使用的PTE）。此外，每一行从左到右依次为：体现PTE树形结构的“..”、PTE在页表中的下标、PTE的值以及从PTE中提取出的物理地址（pa）。最后，不要输出那些无效的PTE。

你的代码可能会输出和样例不同的物理地址，但PTE个数与下标以及虚拟地址应该和上述样例相同。

### 实验内容

1）在kernel/defs.h定义函数原型以便于在exec.c中能够调用。（代码略）

2）根据提示，我们应该将函数写在kernel/vm.c下，可以用到kernel/riscv.h最后的宏定义，并且可以借鉴函数<kbd>freewalk()</kbd>。
通过查看xv6说明书，我们知道xv6中使用的是三级页表，其中前两级页表中PTE解析出的地址为下一级页表的基地址，第三级页表PTE解析出最终的可执行指令的地址。此外，<kbd>freewalk()</kbd>函数使用了深度优先搜索的方式来遍历页表，实现对页表的释放。
为了更好地说明，这里先将代码全部给出：
```c
/* kernel/vm.c */
...
444 void print_pte(pagetable_t pagetable)
445 {
446   // there are 2^9 = 512 PTEs in a page table.
447   for(int i = 0; i < 512; i++){
448     pte_t pte = pagetable[i];
449     if((pte & PTE_V) && (pte & (PTE_R|PTE_W|PTE_X)) == 0){
450       // this PTE points to a lower-level page table.
451       uint64 child = PTE2PA(pte);
452       printf(".. ..%d: pte %p pa %p\n", i, pte, PTE2PA(pte));
453       print_pte((pagetable_t)child);
454     } else if(pte & PTE_V){
455       printf(".. .. ..%d: pte %p pa %p\n", i, pte, PTE2PA(pte));
456     }
457   }
458 }
459
460 void vmprint(pagetable_t pagetable)
461 {
462   printf("page table %p\n", pagetable);
463   for(int i = 0; i < 512; i++){
464     pte_t pte = pagetable[i];
465     if((pte & PTE_V) && (pte & (PTE_R|PTE_W|PTE_X)) == 0){
466       // this PTE points to a lower-level page table.
467       uint64 child = PTE2PA(pte);
468       printf("..%d: pte %p pa %p\n", i, pte, PTE2PA(pte));
469       print_pte((pagetable_t)child);
470     }
471
472   }
473 }
```
由于题目要求第一行输出传入的参数，这不方便加入到递归中，因此使用两个函数来完成题目的打印要求。<kbd>vmprint()</kbd>函数先遍历传入的页表（第三级页表），并判断每个元素是否是有效的，若为有效PTE，则将其打印出来，否则就直接略过。由于第三级页表所有PTE均指向第二级页表，因此在打印后我们调用<kbd>print_pte()</kbd>函数对后续分支进行打印。 
将打印功能拆分成两个函数还有一个优势，即我们可以很方便地打印出树的结构，在<kbd>vmprint()</kbd>函数中，其仅涉及打印第三级页表的PTE，因此printf中“..”长度是固定的。对于<kbd>print_pte()</kbd>函数，我们也仅需简单的分类就可以将剩下的第二级和第一级页表分开打印。 
在<kbd>print_pte()</kbd>函数中，当遇到那些作为下一级页表指针的PTE时，我们不难推出它们一定是第二级页表的元素，因此按照相应的模式打印后，我们递归调用自身，从而追踪指向的第一级页表。相应的，如果遇到那些“叶子结点”，我们也能肯定它们一定是在第一级页表中，打印后即可返回。 
就这样，拆分成两个函数后，我们通过简单的分类和递归就完成了实验的要求。

3）最后，按照实验简介中所述，在exec.c文件里return argc一行前插入：
```c
if(p->pid==1) vmprint(p->pagetable);
```
至此，我们便完成了这个实验的全部要求，运行$ make qemu，得到以下输出：
```bash
xv6 kernel is booting

hart 1 starting
hart 2 starting
page table 0x0000000087f6e000
..0: pte 0x0000000021fda801 pa 0x0000000087f6a000
.. ..0: pte 0x0000000021fda401 pa 0x0000000087f69000
.. .. ..0: pte 0x0000000021fdac1f pa 0x0000000087f6b000
.. .. ..1: pte 0x0000000021fda00f pa 0x0000000087f68000
.. .. ..2: pte 0x0000000021fd9c1f pa 0x0000000087f67000
..255: pte 0x0000000021fdb401 pa 0x0000000087f6d000
.. ..511: pte 0x0000000021fdb001 pa 0x0000000087f6c000
.. .. ..510: pte 0x0000000021fdd807 pa 0x0000000087f76000
.. .. ..511: pte 0x0000000020001c0b pa 0x0000000080007000
init: starting sh
$ QEMU: Terminated
```

## Part II: A kernel page table per process

### 实验简介

当前的xv6只拥有一个单一的kernel pagetable来将kernel的虚拟地址映射成物理地址。此外，每个进程都拥有自己的pagetable，对应着进程自身地址空间的映射关系。由于kernel pagetable中并不含有进程page table中的映射关系，因此kernel并不能直接使用来自用户空间的地址（需要通过系统调用）。我们接下去的任务就是让kernel能够直接访问用户指针所指向的地址。

首先我们需要修改kernel使得每个进程都拥有一份kernel pagetable。为了达成这个目的，你需要在进程的结构体（PCB）中增加*kernel pagetable*一项，然后分别调整进程创建、调度、释放相关的代码来维护它，每个进程的kernel pagetable都应该和全局的kernel pagetable保持一致。为了证明完成了这部分实验，你需要通过usertests的测试。

### 实验内容

1）在struct proc中添加 kernel_pagetable 一项：
```c
/* kernel/proc.h */
...
 85 // Per-process state
 86 struct proc {
...
107   pagetable_t kernel_pagetable;
108 };
```
2）进程创建（allocproc）
根据提示，我们可以模仿<kbd>kvminit()</kbd>来创建每个进程的kernel pagetable。此外，xv6在<kbd>procinit()</kbd>中建立了kernel stack，我们也需要将这个部分功能添加到<kbd>allocproc()</kbd>中。
<kbd>kvminit()</kbd>函数以及其中包含的<kbd>kvmmap()</kbd>函数都是直接对全局kernel pagetable的初始化，因此不需要传入任何参数。为了针对每个进程进行初始化，我们只需要将对象改成新建的指针并返回，并在进程创建后调用我们修改过的初始化函数即可。
添加新的函数后，我们还需要往defs.h中添加定义（<kbd>kvminit_m()</kbd>与<kbd>kvmmap_m()</kbd>），defs.h中的代码放在这个部分实验内容的最后。

```c
/* kernel/vm.c */
...
 56 pagetable_t
 57 kvminit_m()
 58 {
 59   pagetable_t pagetable = (pagetable_t) kalloc();
 60   memset(pagetable, 0, PGSIZE);
 61
 62   // uart registers
 63   kvmmap_m(pagetable, UART0, UART0, PGSIZE, PTE_R | PTE_W);
 64
 65   // virtio mmio disk interface
 66   kvmmap_m(pagetable, VIRTIO0, VIRTIO0, PGSIZE, PTE_R | PTE_W);
 67
 68   // CLINT
 69   kvmmap_m(pagetable, CLINT, CLINT, 0x10000, PTE_R | PTE_W);
 70
 71   // PLIC
 72   kvmmap_m(pagetable, PLIC, PLIC, 0x400000, PTE_R | PTE_W);
 73
 74   // map kernel text executable and read-only.
 75   kvmmap_m(pagetable, KERNBASE, KERNBASE, (uint64)etext-KERNBASE, PTE_R | PT    E_X);
 76
 77   // map kernel data and the physical RAM we'll make use of.
 78   kvmmap_m(pagetable, (uint64)etext, (uint64)etext, PHYSTOP-(uint64)etext, P    TE_R | PTE_W);
 79
 80   // map the trampoline for trap entry/exit to
 81   // the highest virtual address in the kernel.
 82   kvmmap_m(pagetable, TRAMPOLINE, (uint64)trampoline, PGSIZE, PTE_R | PTE_X)    ;
 83
 84   return pagetable;
 85 }
...
164 void
165 kvmmap_m(pagetable_t pagetable, uint64 va, uint64 pa, uint64 sz, int perm)
166 {
167   if(mappages(pagetable, va, sz, pa, perm) != 0)
168     panic("kvmmap_m");
169 }
...
```
对于<kbd>procinit()</kbd>中的部分，我们直接复制到<kbd>allocproc()</kbd>中kvminit_m();语句后即可：
```c
/* kernel/proc.c */
 93 static struct proc*
 94 allocproc(void)
 95 {
...
131   // Initialize kernel pagetable
132   p->kernel_pagetable = kvminit_m();
133
134   // Allocate a page for the process's kernel stack.
135   // Map it high in memory, followed by an invalid
136   // guard page.
137   char *pa = kalloc();
138   if(pa == 0)
139     panic("kalloc");
140   uint64 va = KSTACK((int) (p - proc));
141   kvmmap_m(p->kernel_pagetable, va, (uint64)pa, PGSIZE, PTE_R | PTE_W);
142   p->kstack = va;
143
144   return p;
145 }
```
3）进程调度（scheduler）
根据提示，我们需要将在进程运行时将其kernel pagetable存入到core的*satp*寄存器中，而当没有进程在运行时则存入全局的kernel pagetable。使用并仿造将全局kernel pagetable存入*satp*寄存器的<kbd>kvminithart()</kbd>函数，我们不难写出以下代码：
```c
/* kernel/proc.c */
485 scheduler(void)
486 {
487   struct proc *p;
488   struct cpu *c = mycpu();
...
502         p->state = RUNNING;
503         c->proc = p;
504
505         // Load the process's page table to stap register
506         w_satp(MAKE_SATP(p->kernel_pagetable));
507         swtch(&c->context, &p->context);
508
509         // Process is done running for now.
510         // It should have changed its p->state before coming back.
511
512         // If no process is running
513         kvminithart();
514
515         c->proc = 0;
...
```
4）进程释放（freeproc）
模仿释放进程各自的pagetable的<kbd>proc_freepagetable()</kbd>函数，我们先释放进程的kernel stack，然后再释放进程的kernel pagetable。根据提示，我们不需要释放页表中的“叶子结点”。发现vm.c中的<kbd>free_walk()</kbd>函数可以递归地释放页表的所有节点，因此我们可以适当地对其进行修改，从而避免释放叶子结点。
同样地，我们需要将<kbd>proc_freepagetable_k()</kbd>函数和<kbd>free_walk_m()</kbd>函数添加到defs.h中。
```c
/* kernel/proc.c */
147 // free a proc structure and the data hanging from it,
148 // including user pages.
149 // p->lock must be held.
150 static void
151 freeproc(struct proc *p)
152 {
153   if(p->trapframe)
154     kfree((void*)p->trapframe);
155   p->trapframe = 0;
156   if(p->pagetable)
157     proc_freepagetable(p->pagetable, p->sz);
158
159   if(p->kernel_pagetable)
160     proc_freepagetable_k(p->kernel_pagetable, p->kstack);
161
162   p->pagetable = 0;
163   p->kernel_pagetable = 0;
164   p->sz = 0;
165   p->pid = 0;
166   p->parent = 0;
167   p->name[0] = 0;
168   p->chan = 0;
169   p->killed = 0;
170   p->xstate = 0;
171   p->state = UNUSED;
172 }
...
219 void
220 proc_freepagetable_k(pagetable_t pagetable, uint64 kstack)
221 {
222   uvmunmap(pagetable, kstack, 1, 1);
223   freewalk_m(pagetable);
224 }
...
/* kernel/vm.c */
...
339 // Recursively free page-table pages.
340 // leaf will NOT be removed.
341 void
342 freewalk_m(pagetable_t pagetable)
343 {
344   // there are 2^9 = 512 PTEs in a page table.
345   for(int i = 0; i < 512; i++){
346     pte_t pte = pagetable[i];
347     if(pte & PTE_V) {
348       pagetable[i] = 0;
349       if((pte & (PTE_R|PTE_W|PTE_X)) == 0){
350         // this PTE points to a lower-level page table.
351         uint64 child = PTE2PA(pte);
352         freewalk_m((pagetable_t)child);
353       }
354     }
355   }
356   kfree((void*)pagetable);
357 }
```
5）至此我们就完成了提示中的所有步骤。但是，此时启动xv6，会产生如下报错：
```bash
panic: virtio_disk_intr status
```
经过上网查阅资料，我们发现问题出在系统中<kbd>kvmpa()</kbd>函数依然使用的是全局的kernel pagetable，因此我们需要将其改成当前进程的kernel pagetable，同时添加相关的头文件引用。
```c
/* kernel/vm.c */
...
  8 #include "spinlock.h"
  9 #include "proc.h"
...
175 uint64
176 kvmpa(uint64 va)
177 {
178   uint64 off = va % PGSIZE;
179   pte_t *pte;
180   uint64 pa;
181
182   pte = walk(myproc()->kernel_pagetable, va, 0);
183   if(pte == 0)
184     panic("kvmpa");
185   if((*pte & PTE_V) == 0)
186     panic("kvmpa");
187   pa = PTE2PA(*pte);
188   return pa+off;
189 }
```
终于我们完成了这个部分的实验任务，启动xv6，运行$ usertests，显示ALL TESTS PASSED。

附：defs.h中添加的函数声明：
```c
 88 // proc.c
...
111 void            proc_freepagetable_k(pagetable_t, uint64);
...
161 // vm.c
183 pagetable_t     kvminit_m();
184 void            freewalk_m(pagetable_t);
185 void            kvmmap_m(pagetable_t, uint64, uint64, uint64, int);
```

## Part III: Simplify copyin/copyinstr

### 实验简介

之前的实验中我们仅仅为每一个进程添加了一个kernel pagetable，而在这部分实验中，我们需要将用户pagetable的映射（user mapping）添加到kernel pagetable中。这样带来的一个直接的好处就是我们可以简化一些函数。
kernel中的<kbd>copyin()</kbd>函数从用户指针处读取内存并将其翻译为kernel可以使用的物理地址。当前的函数是通过walk进程pagetable来实现翻译的，而在完成这部分实验后，kernel就可以直接使用来自用户空间的指针。
新的函数（<kbd>copyin_new()</kbd>和<kbd>copyinstr_new()</kbd>）已经为你写好，因此你只需要在添加了user mapping后，直接在原始的函数中引用新的函数即可。 
如果你能通过usertests和make grade中剩下的测试，就代表你完成了这部分的实验。

### 实验内容

1）用<kbd>copyin_new()</kbd>和<kbd>copyinstr_new()</kbd>替换<kbd>copyin()</kbd>和<kbd>copyinstr()</kbd>，并将其添加到defs.h中。（defs.h内容在最后给出）

```c
/* vm.c */
484 int
485 copyin(pagetable_t pagetable, char *dst, uint64 srcva, uint64 len)
486 {
487   /* old code
...
505   */
506   return copyin_new(pagetable, dst, srcva, len);
507 }
...
513 int
514 copyinstr(pagetable_t pagetable, char *dst, uint64 srcva, uint64 max)
515 {
516   /* old code
...
551   */
552   return copyinstr_new(pagetable, dst, srcva, max);
553 }
```
2）撰写映射复制函数（需添加到defs.h中），这里在对kernel pagetable进行walk时我们需要新建表项，因此<kbd>walk()</kbd>最后的参数（alloc）需要设为1；而在user pagetable中我们仅做遍历即可，因此alloc设置为0。 
根据提示，我们需要判断pagetable是否超出了*PLIC*的界线。此外，kernel pagetable中的表项的PTE_U标志位必须为0，这可以借鉴<kbd>uvmclear()</kbd>函数中的位运算实现。
```c
/* vm.c */
428 int
429 uvmcopy_m(pagetable_t kernel_pagetable, pagetable_t user_pagetable, 
uint64 beg, uint64 end)
430 {
431   uint64 i;
432
433   if(beg > end) return -1;
434   if(end > PLIC) return -1;
435   for(i = PGROUNDUP(beg); i < end; i += PGSIZE){
436     pte_t *upte = walk(user_pagetable, i, 0);
437     pte_t *kpte = walk(kernel_pagetable, i, 1);
438     *kpte = *upte & (~PTE_U);
439   }
440   return 0;
441 }
```
3）根据提示，我们需要在user pagetable变动之后将新的映射添加到kernel pagetable中。这里涉及到的需要改动的函数分别为<kbd>fork()</kbd>，<kbd>sbrk()</kbd>和<kbd>exec()</kbd>。 

**fork:**
将子进程的pagetable映射到kernel pagetable中
```c
/* proc.c */
293 int
294 fork(void)
295 {
...
305   // Copy user memory from parent to child.
306   if(uvmcopy(p->pagetable, np->pagetable, p->sz) < 0){
307     freeproc(np);
308     release(&np->lock);
309     return -1;
310   }
311   np->sz = p->sz;
312
313   if(uvmcopy_m(np->kernel_pagetable, np->pagetable, 0, p->sz) < 0){
314       freeproc(np);
315       release(&np->lock);
316       return -1;
317     }
...
341 }
```
**sbrk**
观察sysproc.c中的<kbd>sys_sbrk()</kbd>函数，我们发现其核心在于proc.c中的<kbd>growproc()</kbd>函数，因此我们仅需在后者中进行改动。 
<kbd>growproc()</kbd>函数用来增加或减少用户的内存，因此需要分为增加或减少两种情况。增加时我们利用之前写好的映射复制函数，将新增的页面复制到kernel pagetable中即可。减少时我们则需要仿造<kbd>uvmdealloc()</kbd>函数，对kernel pagetable进行释放。这里我们依旧需要将新的函数加入到defs.h中

```c
/* vm.c */
323 uint64
324 uvmdealloc_m(pagetable_t pagetable, uint64 oldsz, uint64 newsz)
325 {
326   if(newsz >= oldsz)
327     return oldsz;
328
329   if(PGROUNDUP(newsz) < PGROUNDUP(oldsz)){
330     int npages = (PGROUNDUP(oldsz) - PGROUNDUP(newsz)) / PGSIZE;
331     uvmunmap(pagetable, PGROUNDUP(newsz), npages, 0);
332   }
333
334   return newsz;
335 }

/* proc.c */
267 int
268 growproc(int n)
269 {
270   uint sz, beg, end;
271   struct proc *p = myproc();
272
273   sz = p->sz;
274   beg = sz;
275   end = sz + n;
276   if(n > 0){
277     if((sz = uvmalloc(p->pagetable, sz, sz + n)) == 0) {
278       return -1;
279     }
280     if(uvmcopy_m(p->kernel_pagetable, p->pagetable, beg, end) < 0){
281       return -1;
282     }
283   } else if(n < 0){
284     sz = uvmdealloc(p->pagetable, sz, sz + n);
285     uvmdealloc_m(p->kernel_pagetable, beg, end);
286   }
287   p->sz = sz;
288   return 0;
289 }
```
**exec**
当exec完成进程的替换之后，我们先将原来的kernel pagetable释放，再将更新后的进程pagetable作为新的kernel pagetable。
```c
/* exec.c */
 12 int
 13 exec(char *path, char **argv)
...
119   uvmunmap(p->kernel_pagetable, 0, PGROUNDDOWN(oldsz) / PGSIZE, 0);
120   if(uvmcopy_m(p->kernel_pagetable, p->pagetable, 0, p->sz) < 0)
121     goto bad;
122
123   if(p->pid==1) vmprint(p->pagetable);
124   return argc;
...
```
4）最后，在<kbd>userinit()</kbd>中将第一个进程的pagetable的映射关系加入到其kernel pagetable中。
```c
/* proc.c */
238 void
239 userinit(void)
240 {
...
258   p->state = RUNNABLE;
259
260   uvmcopy_m(p->kernel_pagetable, p->pagetable, 0, PGSIZE);
261
262   release(&p->lock);
263 }
264
```
5）至此，我们就完成了这个部分的全部内容，启动xv6，运行$ usertests，显示ALL TESTS PASSED。
附：defs.h中添加的函数声明：
```c
/* defs.h */
161 // vm.c
...
186 int             uvmcopy_m(pagetable_t, pagetable_t, uint64, uint64);
187 uint64          uvmdealloc_m(pagetable_t, uint64, uint64);
188
189 // vmcopyin.c
190 int             copyin_new(pagetable_t, char *, uint64, uint64);
191 int             copyinstr_new(pagetable_t, char *, uint64, uint64);
...
```
## 实验结果
这里出现了一个小插曲：由于使用的虚拟机性能不高，运行usertests时需要花费较多时间，而这将会超出打分程序容忍的时间上限（300s）。所以，虽然在make qemu、启动xv6后运行$ usertests，可以显示测试通过，但在make grade中usertests一项总会显示超时。为了解决这一问题，我强行关闭了打分程序的定时器功能。
```py
# gradelib.py
    def __react(self, reactors, timeout):
        deadline = time.time() + timeout
        try:
            while True:
                timeleft = 1 #deadline - time.time()
                if timeleft < 0:
                    sys.stdout.write("Timeout! ")
                    sys.stdout.flush()
                    return
```
重新测试，终于全部通过。
```bash
== Test pte printout == 
$ make qemu-gdb
pte printout: OK (10.4s) 
== Test answers-pgtbl.txt == answers-pgtbl.txt: OK 
== Test count copyin == 
$ make qemu-gdb
count copyin: OK (5.7s) 
== Test usertests == 
$ make qemu-gdb
(386.6s) 
== Test   usertests: copyin == 
  usertests: copyin: OK 
== Test   usertests: copyinstr1 == 
  usertests: copyinstr1: OK 
== Test   usertests: copyinstr2 == 
  usertests: copyinstr2: OK 
== Test   usertests: copyinstr3 == 
  usertests: copyinstr3: OK 
== Test   usertests: sbrkmuch == 
  usertests: sbrkmuch: OK 
== Test   usertests: all tests == 
  usertests: all tests: OK 
== Test time == 
time: OK 
Score: 66/66
```

## 实验思考题

>Explain why the third test srcva + len < srcva is necessary in copyin_new() : give values for srcva and len for which the first two test fail (i.e., they will not cause to return -1) but for which the third one is true (resulting in returning -1).

由于srcva和len都是无符号64位整数，因此相加时可能会出现溢出问题从而导致得到一个比srcva更小的结果。这个测试条件就是为了防止出现溢出从而产生越界问题。例如：srcva = 1234， len = 2^64 - 1234

## 实验感想

确实感受到了这个实验的难度很高，而有些地方也是在网络上查找了相关资料后才能够顺利进行下去。总结来看，我觉得难度高主要体现在以下两个方面：
1. 对xv6三级页表的使用以及所谓的映射关系不够熟悉。
2. 相较于先前的实验，本实验对原代码的改动较大且改动之处较多，导致我就算跟着提示这样操作了，也会稍感不安。
