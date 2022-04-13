#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
  pid_t pid;
  pid = fork();
  if(pid < 0)
  {
    perror("fork()");
  }
  if(pid == 0)  // gdb 用info inferiors查看所有进程
  {
    printf("this is child,pid = %d\n",getpid());
  }
  else
  {
    printf("this is parent,pid = %d\n",getpid());
  }
  exit(0);
}
