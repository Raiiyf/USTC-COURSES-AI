# OS Lab06: File System

赖永凡 PB20061210
22/12/10

## Part I: Large files

### 实验简介

在这个任务中，你将会扩大xv6所允许的文件的最大值。当前的xv6系统inode中包含12个"direct" block，和1个"singly-indirect" block，所以一共可索引 12+256=268 个blocks。你的任务便是在inode中新增一个"doubly-indirect"。注意到这将会牺牲1个"direct" block，因此最终系统可索引 256*256+256+11=65803 个block。

`bigfile` 测试了系统能够允许的最大文件，它测试通过便意味着你完成了这一部分的实验

### 实验内容

1）修改系统中的常数。注意保持 `inode` 和 `dinode` 结构体在addrs上的一致，以及记得修改系统指定的最大文件数。

```c
/* kernel/file.h */
 16 // in-memory copy of an inode
 17 struct inode {
 18   uint dev;           // Device number
 19   uint inum;          // Inode number
 20   int ref;            // Reference count
 21   struct sleeplock lock; // protects everything below here
 22   int valid;          // inode has been read from disk?
 23
 24   short type;         // copy of disk inode
 25   short major;
 26   short minor;
 27   short nlink;
 28   uint size;
 29   uint addrs[NDIRECT+2];
 30 };
...
/* kernel/fs.h */
 27 #define NDIRECT 11
 28 #define NINDIRECT (BSIZE / sizeof(uint))
 29 #define MAXFILE (NDIRECT + NINDIRECT + NINDIRECT * NINDIRECT)
 30
 31 // On-disk inode structure
 32 struct dinode {
 33   short type;           // File type
 34   short major;          // Major device number (T_DEVICE only)
 35   short minor;          // Minor device number (T_DEVICE only)
 36   short nlink;          // Number of links to inode in file system
 37   uint size;            // Size of file (bytes)
 38   uint addrs[NDIRECT+2];   // Data block addresses
 39 };
```

2）修改 <kbd>bmap()</kbd> 函数。在addrs数组中，前 `NDIRECT`（11）项保存的是"direct" block，即就是block所在的地址；第 `NDIRECT` + 1 项对应"singly-indirect" block，即可以视作一张包含了256个表项的关于block的表，查询时根据元素相对表头的偏移量直接找到对应表项即可；而第 `NDIRECT` + 2 项就是我们要实现的"doubly-indirect" block，可以看成一张包含256个"singly-indirect" block入口的表项，因此需要计算元素位于哪一个"singly-indirect" block，以及偏移量是多少，而这可以通过模256运算来实现。 
具体的查找方式模仿现有的代码即可，具体的思路便是先载入缓存，再读取缓存块中的数据。

```c
/* kernel/fs.c */
377 static uint
378 bmap(struct inode *ip, uint bn)
379 {
380   uint addr, *a;
381   struct buf *bp;
382
383   if(bn < NDIRECT){
384     if((addr = ip->addrs[bn]) == 0)
385       ip->addrs[bn] = addr = balloc(ip->dev);
386     return addr;
387   }
388   bn -= NDIRECT;
389
390   if(bn < NINDIRECT){
391     // Load indirect block, allocating if necessary.
392     if((addr = ip->addrs[NDIRECT]) == 0)
393       ip->addrs[NDIRECT] = addr = balloc(ip->dev);
394     bp = bread(ip->dev, addr);
395     a = (uint*)bp->data;
396     if((addr = a[bn]) == 0){
397       a[bn] = addr = balloc(ip->dev);
398       log_write(bp);
399     }
400     brelse(bp);
401     return addr;
402   }
403   bn -= NINDIRECT;
404
405   if(bn < NINDIRECT * NINDIRECT){
406     // Load doubly-indirect block, allocating if necessary.
407     if((addr = ip->addrs[NDIRECT + 1]) == 0)
408       ip->addrs[NDIRECT + 1] = addr = balloc(ip->dev);
409     bp = bread(ip->dev, addr);
410     a = (uint*)bp->data;
411     uint index = bn / NINDIRECT;
412     uint offset = bn % NINDIRECT;
413
414     // Accessing doubly-indirect block
415     if((addr = a[index]) == 0){
416       a[index] = addr = balloc(ip->dev);
417       log_write(bp);
418     }
419     brelse(bp);
420
421     // Accessing indirect block
422     bp = bread(ip->dev, addr);
423     a = (uint*)bp->data;
424     if((addr = a[offset]) == 0){
425       a[offset] = addr = balloc(ip->dev);
426       log_write(bp);
427     }
428     brelse(bp);
429     return addr;
430   }
431
432   panic("bmap: out of range");
433 }
```

3）修改 <kbd>itrunc()</kbd> 函数，新增释放"doubly-indirect" block的功能，同样模仿已有代码即可。这里是遍历式地删除节点，因此实现起来更为简单。

```c
/* kernel/fs.c */
437 void
438 itrunc(struct inode *ip)
439 {
440   int i, j;
441   struct buf *bp, *bp1;
442   uint *a, *b;
443
444   for(i = 0; i < NDIRECT; i++){
445     if(ip->addrs[i]){
446       bfree(ip->dev, ip->addrs[i]);
447       ip->addrs[i] = 0;
448     }
449   }
450
451   if(ip->addrs[NDIRECT]){
452     bp = bread(ip->dev, ip->addrs[NDIRECT]);
453     a = (uint*)bp->data;
454     for(j = 0; j < NINDIRECT; j++){
455       if(a[j])
456         bfree(ip->dev, a[j]);
457     }
458     brelse(bp);
459     bfree(ip->dev, ip->addrs[NDIRECT]);
460     ip->addrs[NDIRECT] = 0;
461   }
462
463   if(ip->addrs[NDIRECT + 1]){
464     bp = bread(ip->dev, ip->addrs[NDIRECT + 1]);
465     a = (uint*)bp->data;
466     for(i = 0; i < NINDIRECT; i++){
467       if(a[i]){
468         bp1 = bread(ip->dev, a[i]);
469         b = (uint*)bp1->data;
470         for(j = 0; j < NINDIRECT; j++){
471           if(b[j])
472             bfree(ip->dev, b[j]);
473         }
474         brelse(bp1);
475         bfree(ip->dev, a[i]);
476       }
477     }
478     brelse(bp);
479     bfree(ip->dev, ip->addrs[NDIRECT + 1]);
480   }
481
482   ip->size = 0;
483   iupdate(ip);
484 }
485
```

4）至此，我们就完成了这个部分的要求。运行xv6，输入$ bigfile，显示以下输出：

```bash
$ bigfile
................................................................................
................................................................................
................................................................................
................................................................................
................................................................................
................................................................................
................................................................................
................................................................................
..................
wrote 65803 blocks
bigfile done; ok
```

## Part II: Symbolic links

### 实验简介

在这部分的实验中，你需要往xv6中添加 symbolic links。symbolic links 可以链接到某个路径，当 symbolic links 被打开时，内核会打开 symbolic links 链接的路径中的文件。symbolic links 与 hard links 相似，但前者可以指向不同设备中的文件，而后者不行（尽管xv6系统中只有一个设备）。

你将会添加一个新的 system call：<kbd>symlink(char *target, char *path)</kbd> 来实现 symbolic links 的功能。将 symlinktest 添加到 Makefile 中并运行该测试和 usertests，测试通过就代表你完成了这一部分的实验。

### 实验内容

1）新增一个 system call 的基本工作：在 kernel 中声明 `SYS_symlink` ，并添加其到系统调用入口数组中；在用户空间中声明 <kbd>symlink(char *target, char *path)</kbd> 原型，并在 usys.pl 中添加函数入口。最后，将 `symlinktest` 添加到 Makefile 中。（这一部分已经多次操作，代码略）

2）添加 `T_SYMLINK` 和 `O_NOFOLLOW` 的定义，我们将在后面用到这两个变量。因为后续判断时用到了按位相与，所以在添加 `O_NOFOLLOW` 时需注意不要和之前的标志在位上有重叠。

```c
/* stat.h */
  1 #define T_DIR     1   // Directory
  2 #define T_FILE    2   // File
  3 #define T_DEVICE  3   // Device
  4 #define T_SYMLINK 4
...
/* fcntl.h */
  1 #define O_RDONLY  0x000
  2 #define O_WRONLY  0x001
  3 #define O_RDWR    0x002
  4 #define O_CREATE  0x200
  5 #define O_TRUNC   0x400
  6 #define O_NOFOLLOW 0x100
```

3）完善 sysfile.c 中的 <kbd>sys_symlink()</kbd> 函数。有关 symbolic links 的介绍实验手册中并未给太多的说明，比对 <kbd>sys_link()</kbd> 函数和查阅资料后我们可以知道，所谓 symbolic links 其实就是新建一个路径为 path 的 inode，然后将 target 写入inode即可。此外，kernel 中提供的 <kbd>create()</kbd> 和 <kbd>writei()</kbd> 可以用来创建和写入inode，大大方便了我们实现这一功能。 

```c
/* sysfile.c */
509 uint64 sys_symlink(void)
510 {
511   char target[MAXPATH], path[MAXPATH];
512   struct inode *ip;
513
514   if(argstr(0, target, MAXPATH) < 0 || argstr(1, path, MAXPATH) < 0)
515     return -1;
516   begin_op();
517
518   if((ip = create(path, T_SYMLINK, 0, 0)) == 0){
519     end_op();
520     return -1;
521   }
522
523   if((writei(ip, 0, (uint64)target, 0, MAXPATH) != MAXPATH)){
524     end_op();
525     return -1;
526   }
527
528   iupdate(ip);
529   iunlockput(ip);
530   end_op();
531   return 0;
532 }
```

4）修改 <kbd>sys_open()</kbd>函数，使得其能够兼容 symbolic links，根据提示里的要求，如果打开时设置了 O_NOFOLLOW，那么我们就无需跳转；如果链接的文件依然是一个 symbolic link，那么我们需要继续打开其指向的文件，知道访问了一个非 symbolic link 文件为止。注意到这样可能会出现成环而导致该过程无限循环下去，因此我们需要设置一个深度（10）来限制跳转的次数。 
具体的框架便是一个 while 循环加上计数器，循环体内部类似于指针访问那样每一次访问 symbolic link 指向的文件即可。

```c
/* sysfile.c */
...
319   if(ip->type == T_DEVICE && (ip->major < 0 || ip->major >= NDEV)){
320     iunlockput(ip);
321     end_op();
322     return -1;
323   }
324
325   if(ip->type == T_SYMLINK){
326     if(!(omode & O_NOFOLLOW)){
327       int cnt = 0;
328       while(ip->type == T_SYMLINK){
329         cnt++;
330         if(cnt > 10){
331           iunlockput(ip);
332           end_op();
333           return -1;
334         }
335         readi(ip, 0,(uint64)path, 0, MAXPATH);
336         iunlockput(ip);
337         if((ip = namei(path)) == 0){
338           end_op();
339           return -1;
340         }
341         ilock(ip);
342       }
343     }
344   }
346   if((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0){
347     if(f)
348       fileclose(f);
349     iunlockput(ip);
350     end_op();
351     return -1;
352   }
...
```

5）由于提示不要求我们处理 symbolic link 指向文件夹的情况，所以到此为止我们就完成了这一部分的所有要求。运行xv6，输入 $ symlinktest，显示以下结果：

```bash
$ symlinktest
Start: test symlinks
test symlinks: ok
Start: test concurrent symlinks
test concurrent symlinks: ok
```

## 实验结果

本次实验对系统内核改动较小，因此 usertests 时没有出现超时现象，可以正常完成打分：

```bash
== Test running bigfile == 
$ make qemu-gdb
running bigfile: OK (160.3s) 
== Test running symlinktest == 
$ make qemu-gdb
(1.8s) 
== Test   symlinktest: symlinks == 
  symlinktest: symlinks: OK 
== Test   symlinktest: concurrent symlinks == 
  symlinktest: concurrent symlinks: OK 
== Test usertests == 
$ make qemu-gdb
usertests: OK (339.1s) 
== Test time == 
time: OK 
Score: 100/100
```

## 实验感想

本次实验有关 file system，第一部分 inode 相关，由于课堂上有较为详细的介绍，因此做起来比较得心应手。而第二部分的 symbolic link 则较为陌生，且提示较少，在查阅资料后才明白具体功能和要求。
总的来说，本次实验代码量较小，涉及到的数据结构也比较常规，难度适中。此外，作为 OS 的最后一次实验，居然又涉及到了 system call，颇有一种首尾呼应的感觉。
