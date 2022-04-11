#include <time.h>
#include <stdio.h>
int main()
{
  // 系统调用在linux2.6以后不一定会陷入内核，参考man vdso
  printf("%d\n", time(0));
  return 0;
}
