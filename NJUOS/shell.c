#include <unistd.h>
#include <stdbool.h>

// 针对echo hello > /etc/test.txt出现permission denied
// 然后sudo echo hello > /etc/test.txt也会出现permission denied
// 以下为shell的伪代码
int main(int argc, char* argv[])
{
  const char* filename = "test";
  const char* envp = "test";
  bool redirect_stdout = true;
  bool redirect_stderr = false;
  bool bg = true;
  bool status = true;
  pid_t p = fork();
  if(p == 0)
  {
    if(redirect_stdout)
    {
      int fd = open();
      // 由于dup2在execve之前执行的，也就是说文件的重定向在execve之前
      // 所以重定向的文件的权限不是root，因此就算加上sudo也会出现permission denied
      dup2(fd, STDOUT_FILENO);
      close(fd);
    }
    if(redirect_stderr)
    {
      // ...
    }
    execve(filename, argv, envp);
  }
  else
  {
    if(!bg)
    {
      wait(&status);
    }
  }
  return 0;
}
