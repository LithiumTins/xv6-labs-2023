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