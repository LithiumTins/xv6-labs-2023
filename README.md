# MIT 6.S081 Labs

近日学校操作系统课程开始了实验课，目测应该是tiny xv6的样子，并且把完整xv6当做附加题。所以还是决定完成xv6实验，把相关代码上传至此仓库作为备份。<br>
（目前没有做附加题，后面有空了可能会做吧）<br><br>

## Lab1 Utilities
此Lab主要是帮助熟悉环境，包括启动xv6和使用一些系统调用编写内置的用户态程序。学校的实验则是从中选取了一部分实验，外加一个注入xv6启动过程使之打印一些额外语句。应该说学校的这个改动有些莫名奇妙？

Lab1的总体难度不大，有点接触系统调用的经验的话做起来应该会比较轻松。不过我遇上一些抽象的问题，甚至一度以为是kernel的bug，最终还是花费了不少时间😞。<br><br>

## Lab2 System calls
目标应该是进一步了解xv6的运行原理，此Lab包括学习使用gdb和加入两个系统调用。学校还没有来到Lab2，姑且先占个坑吧。

Lab2的内容感觉还是经过精心设计的，加入两个系统调用需要明白xv6中系统调用的完整逻辑，要能理解其中的跳转逻辑，比起Lab1的话代码逻辑倒是简单了不少。

实验的过程中比较让我不爽的是，一开始加入`trace`很自然地把它归类到了`proc`里面，然后`sysinfo`似乎又显得很“怪胎”。想着单开一个system.c把它放进去，但是只有一个`sysinfo`又还是不大舒服，emmm...，或许是我强迫症上来了吧。最后把它俩都放到`mine.c`了，乐。<br><br>

## Lab3 Page tables
xv6的页表实验还是很友好的，`vmprint`仿照`freewalk`的话实现很容易；而`usyscall`的话仿照内核中`TRAPFRAME`，基本上就是它做什么你做什么；最后的实验感觉更像是`syscall`实验 :)

由于是先被学校的页表实验狠狠地折磨，做到xv6实验的时候只觉得无比轻松，有种回到了家的感觉...<br><br>

## Lab4 Traps
关于 `usertrap` 什么的，其实在 `syscall` 的实验中已经有所了解，这个实验主要聚焦于内核态代码和汇编代码之间的交互，如果了解系统调用从 `uservec` 开始的全过程的话倒是并没有太大难度。

`Backtrace` 属实是折磨了我一会儿，一开始以为是打印 `fp` ，然后始终得不到正常的结果，最后才发现是让打印返回地址。 `Alarm` 实验还算是蛮有意思，之前就一直很好奇信号之类的实现来着，这个实验也算是打了个样吧。

## Lab5 Copy on-Write
实验要求实现 `copy-on-write` 的 `fork` ，由于之前被学校的实验折磨过，这个实验做的时候倒是也没遇到太大的困难。先修改 `kalloc` 和 `kfree` ，再修改 `uvmcopy` ，然后触发 `usertrap` 以后就知道会报哪个 `scause` ，一路下来似乎行云流水，除了一些debug以外。唯一感觉抽象的是 `usertests` 里面的 `MAXVAtest` ，会导致 `walk` 的 `panic` ，仔细看一看倒是也不难理解。

话说一直觉得COW是个蛮奇妙的东西，做了这个实验有不错的感觉。

## Lab6 Multithreading
实验要求围绕并发变成完成一些小任务，总体来说算是非常简单的实验，甚至没有怎么debug就通过了。第一题的关键仍然在C代码和汇编的交互上面，把这个看懂了以后实现起来很轻松。后面两题来到了linux主机环境，更是有一种回到了家的温暖（

## Lab7 network driver
实验要求实现E1000网卡驱动程序中的接收和发送程序。虽然它给了400多页全英文的英特尔手册，吓死个人，实际上却几乎是整个xz6最直接，工作量最小的实验。几乎不需要阅读手册，按照指导书的hint按部就班地写下来就好了。

不过有个大坑点，指导书关于加锁的说法很不明确，其实 `e1000_recv()` 不需要加锁的（`whichdev()` 中有说明 `plic` 不允许一个设备同时有两个中断）。反而要是加了锁，由于 `e1000_recv` 向上发送到 `net_rx()` 的时候可能会调用 `net_rx_arp()` ，这个函数在处理时会转而调用 `e1000_transmit()` 并再次获取锁，从而造成 `panic()` 。

## Lab8 Lock
实验要求修改 `kalloc.c` 和 `bio.c` ，使原先使用单个锁的内存块与磁盘缓存块分配机制改为使用多个锁，从而降低锁争用的概率。学校的锁实验基本是一样的，把它copy过来然后解决了之前存在的问题就好了。
- 对于 `kalloc` 来说，是比较显然的，根据 `cpuid` 找到对应的链表和锁，如果自己的链表没有内存块就去窃取。注意先释放自己的锁，再获取别的cpu的锁，否则会造成死锁。找到内存块以后可以直接给到调用者，不需要先放回自己的链表。
- 对于 `bio` 来说，由于窃取完成以后需要把缓存块放回到自己的哈希桶，所以跟 `kalloc` 存在一些区别。由于为避免死锁先释放了自己桶的锁，在重新拿到锁之前，可能已经有别的cpu为同一个磁盘块分配了缓存块，如果不管它直接把窃取到的缓存块放回自己的桶，会破坏缓存一致性。所以拿到锁以后先检查是否已经有缓存块，如果有就把窃取的缓存块当成空闲块放回自己的桶，然后返回那个缓存块。

## Lab9 File system
实验要求为添加大文件和符号链接的支持。大文件倒是蛮直接的，因为给出了一级间接索引的代码，所以二级简介索引的代码就多做一层即可。符号链接则折磨一些，需要先对xv6的文件系统实现有一定的了解，知道各种情况是否加锁、提交事务等等。另外，这个实验分配6万多个文件块，跑的速度很慢，以至于我的机器上bigfile的test以及usertests直接超过了时间限制，我把时间限制修改了以后才能过grade。