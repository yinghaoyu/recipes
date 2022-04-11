#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/syscall.h>
#include <string.h>

void func()
{
  printf("good bye\n");
}

int main(int argc, char *argv[])
{
  atexit(func);
  if(argc < 2)
  {
    return EXIT_FAILURE;
  }

  if(strcmp(argv[1], "exit") == 0)
  {
    exit(0);
  }

  if(strcmp(argv[1], "_exit") == 0)
  {
    _exit(0); // 系统调用, 看不到libc注册的函数, strace ./exit-demo _exit可以看调用了exit_group(0)会停掉所有线程
  }
  
  if(strcmp(argv[1], "__exit") == 0)
  {
    syscall(SYS_exit, 0); // 系统调用，
  }
  return 0;
}
