# GCC Analysis

## 预处理
```
gcc -E test.c -o test.i
```

## 编译
```
gcc -S test.i -o test.s
```

## 汇编
```
gcc -c test.s -o test.o
```

## 链接
```
gcc test.o -o test
```

## CFI
Call Frame Information 调用栈帧信息

.cfi_ 用于调试的信息，可以在调用失败的时候回溯栈

A--> B --> C(异常)
  ```
  -fno-asynchronous-unwind-tables
  ```

## PIC
Position Indepentent Code /位置无关的代码
动态链接
```
mov eax, eip ; eip对于编程的人是透明的。不过调用__x86.get_pc_thunk.ax函数可以获取eip的值
```
```
-fno-pic
```

## 栈对齐
-16 = 0xfffffff0
```
andl $-16, esp
```
将栈对齐到16字节
```
-mpreferred-stack-boundary=2
```

## Intel
gcc汇编默认为GAS(GNU Assembler)格式，左边为源地址，右边为目的地址

使用intel的汇编格式，左变为目的地址，右边为源地址
```
-masm=intel
```
