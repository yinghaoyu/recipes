```c
#include <stdio.h>

void delay()
{
  for(violate int i = 0; i < 10000; i++)  // 不加violate可能会被编译器优化掉
  {
  }
}

int main()
{
  delay();
  return 0;
}
```
