#include <unistd.h>
#include <stdio.h>

#define n 2

// 直接执行会输出6次
// p13-fork | wc -l 会输出8次，因为printf有libc维护的缓冲区

int main()
{
  for(int i = 0; i < n; i++)
  {
    fork();
    printf("hello world\n");
  }
  return 0;
}
