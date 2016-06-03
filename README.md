/*
*  
*  原文链接：http://www.sanwho.com/133.html
*  参考博客: http://blog.csdn.net/myarrow/article/details/9630377
*/

Android下so注入是基于ptrace系统调用，因此要想学会android下的so注入，首先需要了解ptrace的用法。

ptrace用法可以参考博客：http://blog.sina.com.cn/s/blog_4ac74e9a0100n7w1.html，也可以在ubuntu下输入man ptrace命令，查看具体描述。

android中进程系统调用劫持可参考博客：http://www.kanxue.com/bbs/showthread.php?t=180461，这是一个android简单的ptrace监控远程进程监控调用的例子。

Android系统是基于Linux系统，在linux系统中可以通过ptrace系统调用实现进程注入。ptrace注入过程大致过程如下：

(1)基于shellcode加载

[1]编写shellcode，shellcode是使用汇编语言写一段汇编程序，该程序实现so库的加载、so库函数查找以及执行库中的函数。

[2]通过远程进程pid，ATTACH到远程进程。

[3]获取远程进程寄存器值，并保存，以便注入完成后恢复进程原有状态。

[4]获取远程进程系统调用mmap、dlopen、dlsym调用地址。

[5]调用远程进程mmap分配一段存储空间，并在空间中写入shellcode、so库路径以及函数调用参数。

[6]执行远程进程shellcode代码。

[7]恢复远程进程寄存器。

[8]detach远程进程。

基于shellcode注入可看雪古河大哥写的libInject,网址: http://bbs.pediy.com/showthread.php?t=141355

(2)直接加载

[1]通过远程进程pid，ATTACH到远程进程。

[2]获取远程进程寄存器值，并保存，以便注入完成后恢复进程原有状态。

[3]获取远程进程系统调用mmap、dlopen、dlsym调用地址。

[4]调用远程进程mmap分配一段存储空间，并在空间中写入so库路径以及函数调用参数。

[5]执行远程进程dlopen,加载so库。

[6]执行远程进程dlsym，获取so库中需要执行的函数地址。

[7]执行远程进程中的函数。

[7]恢复远程进程寄存器。

[8]DETACH远程进程。

目前android  so注入的版本基本上都是基于古河大哥的libInject修改而来。关于so注入的项目，还可以参考洗大师的一个开源项目，网址：https://code.google.com/p/libandroidinjector/downloads/list。洗大师注入需要修改elf文件。





