#include <stdio.h>
#include <stdbool.h>

int main()
{
  while(true)
  {
    // 设置进程的nice值（优先级）
    // nice的值介于-20~19直接，值越小分配的cpu占比越大
    // renice -n [value] [pid]
    // 绑定进程到cpu上
    // taskset -cp [cpuid] [pid]
  }
  return 0;
}
