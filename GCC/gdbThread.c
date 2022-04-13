#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
int a = 0;
int b = 0;
void *thread1_func(void *p_arg)
{
  while (1)
  {
    a++;
    sleep(1);
  }
}

void *thread2_func(void *p_arg)
{
  while (1)
  {
    b++;
    sleep(1);
  }
}

int main(void)
{
  pthread_t t1, t2;

  pthread_create(&t1, NULL, thread1_func, "Thread 1");
  pthread_create(&t2, NULL, thread2_func, "Thread 2");

  sleep(1000);
  return;
}
