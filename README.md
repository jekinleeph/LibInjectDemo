/*
*  
*  ԭ�����ӣ�http://www.sanwho.com/133.html
*  �ο�����: http://blog.csdn.net/myarrow/article/details/9630377
*/

Android��soע���ǻ���ptraceϵͳ���ã����Ҫ��ѧ��android�µ�soע�룬������Ҫ�˽�ptrace���÷���

ptrace�÷����Բο����ͣ�http://blog.sina.com.cn/s/blog_4ac74e9a0100n7w1.html��Ҳ������ubuntu������man ptrace����鿴����������

android�н���ϵͳ���ýٳֿɲο����ͣ�http://www.kanxue.com/bbs/showthread.php?t=180461������һ��android�򵥵�ptrace���Զ�̽��̼�ص��õ����ӡ�

Androidϵͳ�ǻ���Linuxϵͳ����linuxϵͳ�п���ͨ��ptraceϵͳ����ʵ�ֽ���ע�롣ptraceע����̴��¹������£�

(1)����shellcode����

[1]��дshellcode��shellcode��ʹ�û������дһ�λ����򣬸ó���ʵ��so��ļ��ء�so�⺯�������Լ�ִ�п��еĺ�����

[2]ͨ��Զ�̽���pid��ATTACH��Զ�̽��̡�

[3]��ȡԶ�̽��̼Ĵ���ֵ�������棬�Ա�ע����ɺ�ָ�����ԭ��״̬��

[4]��ȡԶ�̽���ϵͳ����mmap��dlopen��dlsym���õ�ַ��

[5]����Զ�̽���mmap����һ�δ洢�ռ䣬���ڿռ���д��shellcode��so��·���Լ��������ò�����

[6]ִ��Զ�̽���shellcode���롣

[7]�ָ�Զ�̽��̼Ĵ�����

[8]detachԶ�̽��̡�

����shellcodeע��ɿ�ѩ�źӴ��д��libInject,��ַ: http://bbs.pediy.com/showthread.php?t=141355

(2)ֱ�Ӽ���

[1]ͨ��Զ�̽���pid��ATTACH��Զ�̽��̡�

[2]��ȡԶ�̽��̼Ĵ���ֵ�������棬�Ա�ע����ɺ�ָ�����ԭ��״̬��

[3]��ȡԶ�̽���ϵͳ����mmap��dlopen��dlsym���õ�ַ��

[4]����Զ�̽���mmap����һ�δ洢�ռ䣬���ڿռ���д��so��·���Լ��������ò�����

[5]ִ��Զ�̽���dlopen,����so�⡣

[6]ִ��Զ�̽���dlsym����ȡso������Ҫִ�еĺ�����ַ��

[7]ִ��Զ�̽����еĺ�����

[7]�ָ�Զ�̽��̼Ĵ�����

[8]DETACHԶ�̽��̡�

Ŀǰandroid  soע��İ汾�����϶��ǻ��ڹźӴ���libInject�޸Ķ���������soע�����Ŀ�������Բο�ϴ��ʦ��һ����Դ��Ŀ����ַ��https://code.google.com/p/libandroidinjector/downloads/list��ϴ��ʦע����Ҫ�޸�elf�ļ���





