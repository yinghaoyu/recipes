#include "thread.h"
#include <stdio.h>

void* fun1()
{
  while(1)
  {
    printf("A");
  }
}

void* fun2()
{
  while(1)
  {
    printf("B");
  }
}

int main()
{
  create(fun1);
  create(fun2);
  return 0;
}
