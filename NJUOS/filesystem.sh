```
fork()后父子进程会共享一个文件描述符的游标，例如父子进程分别写入abc，会看到两个abc，而不是覆盖
```

```bash
vim a.txt
ln a.txt b.txt // 将a.txt文件硬链接到b.txt此时修改b.txt，a.txt也会跟着修改
ln -s c.txt d.txt // -s表示软链接，软链接文件不存在也能成功，如果去掉-s变成硬链接就会失败
ls -li // 可以发现a.txt和b.txt除了文件名外，分不清楚谁先创建
strace rm -rf a.txt // 可以发现调用的是unlink，直到引用计数为0才会真正回收文件
```
