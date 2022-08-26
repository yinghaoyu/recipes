#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

pthread_mutex_t mutex;

void *fun(void *arg)
{
  pthread_mutex_lock(&mutex);
  printf("fun get lock\n");
  sleep(3);

  pthread_mutex_unlock(&mutex);
  printf("fun unlock\n");
  return NULL;
}

int main()
{
  pthread_mutex_init(&mutex, NULL);
  pthread_t id;
  pthread_create(&id, NULL, fun, NULL);

  sleep(1);
  // 这里thread已经获得了mutex，fork后子进程的mutex也是lock状态
  pid_t pid = fork();
  if (pid == -1)
  {
    perror("fork err");
    return -1;
  }

  if (pid == 0)
  {
    pthread_mutex_lock(&mutex);
    printf("child get lock\n");
    sleep(3);

    pthread_mutex_unlock(&mutex);
    printf("child unlock\n");
    exit(0);
  }

  wait(NULL);

  pthread_mutex_destroy(&mutex);
  printf("main over\n");
  return 0;
}
