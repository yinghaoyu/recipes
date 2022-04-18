```
fork()后父子进程会共享一个文件描述符的游标，例如父子进程分别写入abc，会看到两个abc，而不是覆盖
```

```bash
vim a.txt
ln a.txt b.txt // 将a.txt文件软链接到b.txt此时修改b.txt，a.txt也会跟着修改
```
