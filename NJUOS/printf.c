#include <stdio.h>

int main()
{
  // 可以参考setbuf
  // unbuf执行玩printf就是输出
  // linebuf遇到换行就输出（这是终端的输出格式）
  // fullybuf整个程序结束就输出（这是非终端的输出格式）
  printf("Hello, %s\n", "world");
  // 如果没有\n则终端无输出
  // 就算有换行符./printf > a.txt可以发现a.txt也没有输出
  *(int *)NULL = 0;  // 制造sigsegv 
  return 0;
}
