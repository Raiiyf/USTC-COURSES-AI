# OS lab05: Locks

赖永凡 PB20061210
22/12

## Part I: Memory allocator

### 实验简介

在xv6中，进程通过调用<kbd>kalloc()</kbd>和<kbd>kfree()</kbd>来获得/释放内存。然而，由于当前系统中只存在一个全局的kmem以及相应的kmem lock，如果出现多个进程同时调用<kbd>kalloc()</kbd>和<kbd>kfree()</kbd>的话，就会出现拥塞现象，导致处理速度大幅下降。于是，本实验设计了kalloctest来测试内存分配的并行程度，测试中统计并输出等待lock的总轮数。 
本实验的核心问题便在于当前仅有一个freelist，为了增加并行度，你需要改成每个CPU维护一个freelist，并且在<kbd>initlock()</kbd>中初始化所有的lock。值得注意的是，当某个CPU的可用空间不足时，需要从别的CPU中“偷”过来一页。 
运行\$ kalloctest 来测试你是否减少了冲突程度，运行\$ usertests sbrkmuch 来检验系统是否仍然能分配所有的内存，最后，确保usertests中的所有测试能够通过。 

### 实验内容

1）每个CPU一个kmem，NCPU定义了系统的cpu数量。

```c
/* kalloc.c */
 23 struct {
 24   struct spinlock lock;
 25   struct run *freelist;
 26 } kmem[NCPU];
```

2）由于我们将kmem变成了一个数组，自然地我们需要将原先代码中的kmem也改成数组的模式。根据提示，我们可以使用<kbd>cpuid()</kbd>来获得当期cpu的id，此外使用该函数时还需要调用<kbd>push_off()</kbd>和<kbd>pop_off()</kbd>来关闭、开启中断功能。

```c
/* kalloc.c */
 54 void
 55 kfree(void *pa)
 56 {
 57   struct run *r;
 58
 59   if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
 60     panic("kfree");
 61
 62   // Fill with junk to catch dangling refs.
 63   memset(pa, 1, PGSIZE);
 64
 65   r = (struct run*)pa;
 66
 67   push_off();
 68   int id = cpuid();
 69   pop_off();
 70
 71   acquire(&kmem[id].lock);
 72   r->next = kmem[id].freelist;
 73   kmem[id].freelist = r;
 74   release(&kmem[id].lock);
 75 }
...
 99 kalloc(void)
100 {
101   struct run *r;
102
103   push_off();
104   int id = cpuid();
105   pop_off();
106
107   acquire(&kmem[id].lock);
108   r = kmem[id].freelist;
109   if(r)
110     kmem[id].freelist = r->next;
111   else { // steal from other cpu
112     r = steal(id);
113   }
114   release(&kmem[id].lock);
115
116   if(r)
117     memset((char*)r, 5, PGSIZE); // fill with junk
118   return (void*)r;
119 }
```

3）当在本cpu的freelist找不到可用空间时（即得到的*r*为空），我们需要通过<kbd>steal()</kbd>函数来获得其他cpu的可用空间。<kbd>steal()</kbd>需要传入当前cpu的id从而避免重复的寻找，而具体的查找其实和<kbd>kalloc()</kbd>中的方法一致。此外，一旦我们找到了一个可用的空间，我们便可停止查找并返回。

```c
/* kalloc.c */
121 void *
122 steal(int id)
123 {
124   struct run *r;
125   for(int i=0; i < NCPU; i++) {
126     if(i == id) continue;
127     acquire(&kmem[i].lock);
128     r = kmem[i].freelist;
129     if(r)
130       kmem[i].freelist = r->next;
131     release(&kmem[i].lock);
132
133     if(r)
134       return (void*)r;
135   }
136   return (void*)r;
137 }
```

4）最后是初始化的环节。原先我们仅需初始化一个kmem，并为其free所有可用的空间（end到PHYSTPOP）。在修改成每个CPU一个kmem后，我们需要依次初始化每个kmem，并且将可用空间平均分给每一个CPU。为了实现这一功能，我们需要修改<kbd>freerange()</kbd>和新增一个<kbd>kfree_m()</kbd>函数，从而让两个功能与cpu id相适配。 
值得一提的是，提示中告诉我们去使用*kernel/sprintf.c*中的<kbd>snprintf()</kbd>函数来实现format string，从而在<kbd>initlock()</kbd>时能使用不同的名字。但是，提示中同样指出，<kbd>initlock()</kbd>中全部使用*'kmem'*也可以，为了简便起见，这里采用后一种方式。

```c
/* kalloc.c */
 28 void
 29 kinit()
 30 {
 31   uint64 length = (PHYSTOP - (uint64)end) / NCPU;
 32   for(int i = 0; i < NCPU; i++) {
 33     initlock(&kmem[i].lock, "kmem");
 34     uint64 start_add, end_add;
 35     start_add = (uint64)end + i * length;
 36     end_add = start_add + length;
 37     freerange((void*)start_add, (void*)end_add, i);
 38   }
 39 }
 40
 41 void
 42 freerange(void *pa_start, void *pa_end, int id)
 43 {
 44   char *p;
 45   p = (char*)PGROUNDUP((uint64)pa_start);
 46   for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
 47     kfree_m(p, id);
 48 }
 49
...
 77 void
 78 kfree_m(void *pa, int id)
 79 {
 80   struct run *r;
 81
 82   if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
 83     panic("kfree");
 84
 85   // Fill with junk to catch dangling refs.
 86   memset(pa, 1, PGSIZE);
 87
 88   r = (struct run*)pa;
 89
 90   acquire(&kmem[id].lock);
 91   r->next = kmem[id].freelist;
 92   kmem[id].freelist = r;
 93   release(&kmem[id].lock);
 94 }
```

5）至此，我们就完成了这部分实验的主要内容。运行xv6，输入$ kalloctest，得到以下输出：
```bash
$ kalloctest 
start test1
test1 results:
--- lock kmem/bcache stats
lock: kmem: #fetch-and-add 0 #acquire() 115971
lock: kmem: #fetch-and-add 0 #acquire() 139692
lock: kmem: #fetch-and-add 0 #acquire() 156896
lock: kmem: #fetch-and-add 0 #acquire() 4090
lock: kmem: #fetch-and-add 0 #acquire() 4090
lock: kmem: #fetch-and-add 0 #acquire() 4090
lock: kmem: #fetch-and-add 0 #acquire() 4090
lock: kmem: #fetch-and-add 0 #acquire() 4090
lock: bcache: #fetch-and-add 0 #acquire() 1248
--- top 5 contended locks:
lock: proc: #fetch-and-add 780302 #acquire() 220752
lock: proc: #fetch-and-add 403916 #acquire() 220991
lock: proc: #fetch-and-add 354025 #acquire() 220752
lock: proc: #fetch-and-add 288647 #acquire() 220752
lock: proc: #fetch-and-add 273065 #acquire() 220753
tot= 0
test1 OK
start test2
total free number of pages: 32492 (out of 32768)
.....
test2 OK
```
运行$ usertests ，显示ALL TESTS PASSED。

## Part II: Buffer cache

### 实验简介

这半部分的实验需要你改善Buffer Cache相关的lock。原始的xv6系统对整个buffer cache使用了同一把锁，因此一次只能有一个进程访问bcache。类似于前一部分的测试，`bcachetest`评估了当多个进程并发访问bcache时，系统的拥塞程度。 

文件 kernel/bio.c 中的代码详细描述了bcache的工作方式， 你需要修改<kbd>bget()</kbd>和<kbd>breles()</kbd>，使得部分对bcache的并发查询和释放可以同时进行。最后，运行\$ bcachetest来检验你是否完成了这一部分的实验。

### 实验内容

1）根据提示，我们可以利用hash原理，将bcache散列到多个bucket中，并为每一个桶设初始化一把锁来实现一定程度的并发。此外，为了减少hash冲突，我们可以将bucket的个数设置为某个质数。值得一提的是，这里我们保留了整个bcache的lock，这个锁将在后续发挥作用。

```c
/* bio.c */
 26 struct {
 27   struct spinlock lock;
 28   struct buf buf[NBUF];
 29
 30   // Linked list of all buffers, through prev/next.
 31   // Sorted by how recently the buffer was used.
 32   // head.next is most recent, head.prev is least.
 33   struct buf bucket[NBUCKET];
 34   struct spinlock lock_bucket[NBUCKET];
 35 } bcache;
/* bio.h */
 15   #define NBUCKET 13
```

2）这一部分开始我的做法便与提示中不完全相同。原始xv6的bcache包含了一个双向的循环链表，而越靠近链表头的节点，表示其调入bcache的时间越近。而提示中希望我们放弃复杂的链表结构，而在block的结构体中添加time_stamp一项来记载其调入bcache的时间。 
提示中的做法固然更为简便，毕竟其舍弃了复杂的链表结构，但经由测试，使用原先的结构依然能够达成实验目的，而且对代码的改动更少，所以我还是采用了原先的双向循环链表结构。因此，对于数据初始化部分，我们需要将所有bucket中的lock初始化以及将整个buf数组平均分配到每一个bucket中的双向链表中。这里平均分配的方法和<kbd>kinit()</kbd>中的类似。
```c
/* bio.c */
 57 void
 58 binit(void)
 59 {
 60   struct buf *b;
 61
 62   initlock(&bcache.lock, "bcache");
 63
 64   for(int i=0; i<NBUCKET; i++) {
 65     initlock(&bcache.lock_bucket[i], "bcache");
 66
 67     bcache.bucket[i].prev = &bcache.bucket[i];
 68     bcache.bucket[i].next = &bcache.bucket[i];
 69
 70     for(b = bcache.buf + NBUF/NBUCKET * i; b < bcache.buf + NBUF/NBUCKET * (i + 1); b++){
 71       b->next = bcache.bucket[i].next;
 72       b->prev = &bcache.bucket[i];
 73       initsleeplock(&b->lock, "buffer");
 74       bcache.bucket[i].next->prev = b;
 75       bcache.bucket[i].next = b;
 76     }
 77   }
 78 }
```

3）由于保留了原先的链表结构，因此对于下列函数，我们仅需将其改成每个bucket一把锁的结构即可，并无新内容的添加。

```c
/* bio.c */
 void
167 brelse(struct buf *b)
168 {
169   if(!holdingsleep(&b->lock))
170     panic("brelse");
171
172   releasesleep(&b->lock);
173   int key = b->blockno % NBUCKET;
174   acquire(&bcache.lock_bucket[key]);
175   b->refcnt--;
176   if (b->refcnt == 0) {
177     // no one is waiting for it.
178     b->next->prev = b->prev;
179     b->prev->next = b->next;
180     b->next = bcache.bucket[key].next;
181     b->prev = &bcache.bucket[key];
182     bcache.bucket[key].next->prev = b;
183     bcache.bucket[key].next = b;
184   }
185
186   release(&bcache.lock_bucket[key]);
187 }
188
189 void
190 bpin(struct buf *b) {
191   int key = b->blockno % NBUCKET;
192   acquire(&bcache.lock_bucket[key]);
193   b->refcnt++;
194   release(&bcache.lock_bucket[key]);
195 }
196
197 void
198 bunpin(struct buf *b) {
199   int key = b->blockno % NBUCKET;
200   acquire(&bcache.lock_bucket[key]);
201   b->refcnt--;
202   release(&bcache.lock_bucket[key]);
203 }
```

4）最后，我们需要完成<kbd>bget()</kbd>函数的修改。<kbd>bget()</kbd>函数的作用是查询指定的block是否被缓存，若没有的话则需要释放一块已有的cache来缓存该block。
当我们改成每个bucket存一部分bcache后，上述流程就转化为：首先在hash值对应的的bucket中查询block是否在其中，若没有命中，则需要二次查找本bucket，检查是否有可以释放的block。如果还是失败，则需要遍历其余的所有bucket，在其中寻找一块可以使用的block，并将此block对应的链表节点移动到hash值对应的bucket中。 由于我们仍然使用了之前的链表结构，因此查找可释放链表时应从尾端查起，而插入新节点则应当使用头插法。
当然，更为关键的便是锁的运用。在上述步骤中，在本bucket中查找时，需要维护本bucket的锁。然而当前往其他bucket并拆分链表时，如果我们依旧仅锁上相应的bucket，那么将会出现**死锁**现象。例如，当block A想访问B链表，而block B同时想访问A链表时，死锁就发生了。为了避免出现这种情况，我们改成当跨bucket访问时，需要拥有整个bcache的锁。换言之，当需要跨bucket访问时，整个bucket中仅包含此进程，这样就避免了上述情况的出现。

```c
/* bio.c */
 static struct buf*
 83 bget(uint dev, uint blockno)
 84 {
 85   struct buf *b;
 86   int key = blockno % NBUCKET;
 87   acquire(&bcache.lock_bucket[key]);
 88
 89   // Is the block already cached?
 90   for(b = bcache.bucket[key].next; b != &bcache.bucket[key]; b = b->next){
 91     if(b->dev == dev && b->blockno == blockno){
 92       b->refcnt++;
 93       release(&bcache.lock_bucket[key]);
 94       acquiresleep(&b->lock);
 95       return b;
 96     }
 97   }
 98
 99   // Not cached.
100   // Recycle the least recently used (LRU) unused buffer.
101   for(b = bcache.bucket[key].prev; b != &bcache.bucket[key]; b = b->prev){
102     if(b->refcnt == 0) {
103       b->dev = dev;
104       b->blockno = blockno;
105       b->valid = 0;
106       b->refcnt = 1;
107       release(&bcache.lock_bucket[key]);
108       acquiresleep(&b->lock);
109       return b;
110     }
111   }
112
113   // Recycle other bucket
114   // steal buf block
115   release(&bcache.lock_bucket[key]);// release previous lock first
116   acquire(&bcache.lock); // maintain global lock
117   for(int i = 0; i < NBUCKET; i++) {
118     if(i == key) continue;
119     for(b = bcache.bucket[i].prev; b != &bcache.bucket[i]; b = b->prev) {
120       if(b->refcnt == 0) { // move node to cache.bucket[key]
121         b->next->prev = b->prev;
122         b->prev->next = b->next;
123
124         b->next = bcache.bucket[key].next;
125         b->prev = &bcache.bucket[key];
126         bcache.bucket[key].next->prev = b;
127         bcache.bucket[key].next = b;
128         b->dev = dev;
129         b->blockno = blockno;
130         b->valid = 0;
131         b->refcnt = 1;
132         release(&bcache.lock);
133         acquiresleep(&b->lock);
134         return b;
135       }
136     }
137   }
138   panic("bget: no buffers");
139 }
```

5）这样我们就完成了这个部分的实验要求，运行xv6，执行\$ bcachetest，得到以下输出：

```bash
$ bcachetest
start test0
test0 results:
--- lock kmem/bcache stats
lock: kmem: #fetch-and-add 0 #acquire() 4293
lock: kmem: #fetch-and-add 0 #acquire() 4157
lock: kmem: #fetch-and-add 0 #acquire() 4127
lock: kmem: #fetch-and-add 0 #acquire() 4090
lock: kmem: #fetch-and-add 0 #acquire() 4089
lock: kmem: #fetch-and-add 0 #acquire() 4090
lock: kmem: #fetch-and-add 0 #acquire() 4089
lock: kmem: #fetch-and-add 0 #acquire() 4090
lock: bcache: #fetch-and-add 0 #acquire() 2108
lock: bcache: #fetch-and-add 0 #acquire() 4118
lock: bcache: #fetch-and-add 0 #acquire() 4322
lock: bcache: #fetch-and-add 0 #acquire() 6328
lock: bcache: #fetch-and-add 0 #acquire() 6322
lock: bcache: #fetch-and-add 0 #acquire() 6310
lock: bcache: #fetch-and-add 0 #acquire() 6592
lock: bcache: #fetch-and-add 0 #acquire() 6612
lock: bcache: #fetch-and-add 0 #acquire() 6920
lock: bcache: #fetch-and-add 0 #acquire() 4118
lock: bcache: #fetch-and-add 0 #acquire() 4118
lock: bcache: #fetch-and-add 0 #acquire() 2108
lock: bcache: #fetch-and-add 0 #acquire() 4116
--- top 5 contended locks:
lock: virtio_disk: #fetch-and-add 2692387 #acquire() 1095
lock: proc: #fetch-and-add 938550 #acquire() 81940
lock: proc: #fetch-and-add 883421 #acquire() 82288
lock: proc: #fetch-and-add 697877 #acquire() 81940
lock: proc: #fetch-and-add 689945 #acquire() 81940
tot= 0
test0: OK
start test1
test1 OK
```

## 实验结果

由于实验用虚拟机性能不足，这里在make grade时也出现了timeout的情况。和之前的做法类似，我们强行关闭了grade程序的计时器。再次运行打分程序，得到以下输出：

```bash
== Test running kalloctest == 
$ make qemu-gdb
(307.2s) 
== Test   kalloctest: test1 == 
  kalloctest: test1: OK 
== Test   kalloctest: test2 == 
  kalloctest: test2: OK 
== Test kalloctest: sbrkmuch == 
$ make qemu-gdb
kalloctest: sbrkmuch: OK (31.4s) 
== Test running bcachetest == 
$ make qemu-gdb
(48.9s) 
== Test   bcachetest: test0 == 
  bcachetest: test0: OK 
== Test   bcachetest: test1 == 
  bcachetest: test1: OK 
== Test usertests == 
$ make qemu-gdb
usertests: OK (424.0s) 
== Test time == 
time: OK 
Score: 70/70
```

## 实验感想

这个实验总的来说难度适中，既没有难以理解的数据结构，也没有太复杂的工作机制。然而，和上一个实验一样，本实验也涉及到了对原代码的大幅度改动，因此在实现时还是多少有些不够自信。此外，在后一部分中我也第一次尝试脱离提示独立完成（有和同学相互讨论），虽然过程中出现了一些bug，但最终得以平稳落地。
