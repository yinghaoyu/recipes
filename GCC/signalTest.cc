#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MCHECK(ret)      \
  ({                     \
    auto errnum = (ret); \
    assert(errnum == 0); \
    (void) errnum;       \
  })

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv = PTHREAD_COND_INITIALIZER;
int predicate = 0;
unsigned long nwakeup = 0;    // number of wakeup
unsigned long nspurious = 0;  // number of spurious wakeup

bool running = true;

void *thread(void *ignore)
{
  while (running)
  {
    // wait that predicate becomes true
    //
    MCHECK(pthread_mutex_lock(&mutex));
    while (predicate == 0)
    {
      MCHECK(pthread_cond_wait(&cv, &mutex));
      nwakeup++;  // we've been wakeup
      if (predicate == 0)
        nspurious++;  // we got a spurious wakeup
    }

    // reset predicate to false
    //
    predicate = 0;
    MCHECK(pthread_mutex_unlock(&mutex));
  }

  // never reached
  return NULL;
}

void *signal_thread(void *ignore)
{
  while (running)
  {
    // set the predicate to true and wakeup one thread
    //
    MCHECK(pthread_mutex_lock(&mutex));
    predicate = 1;
    // 假设T1是本线程，T2卡在pthread_cond_wait，T3卡在pthread_mutex_lock
    // 单核cpu上，解锁之前唤醒，切换到另一线程T2，此时由于此线程没有解锁，T2从pthread_cond_wait返回时需要重新获取锁，就会阻塞。这时存在不必要的上下文切换。
    // rc = pthread_cond_signal(&cv);  // signal before unlock
    MCHECK(pthread_mutex_unlock(&mutex));
    // 在解锁之后唤醒，如果存在T3线程卡在pthread_mutex_lock，这时候刚好解锁
    // 由于predicate == 1就会把predicate 置 0
    // 这个时候T1的signal到达，T2从pthread_cond_wait返回时，predicate==0，形成了surprious
    // weakup
    MCHECK(pthread_cond_signal(&cv));  // signal after unlock
  }

  // never reached
  return NULL;
}

const int NTHREADS = 8;  // # threads waiting on the condvar

int main()
{
  pthread_t tid[NTHREADS];  // threads waiting on  the condvar
  pthread_t tsig;           // thread that signals the condvar

  // create our threads
  //
  for (int i = 0; i < NTHREADS; i++)
  {
    MCHECK(pthread_create(&tid[i], NULL, thread, NULL));
  }
  MCHECK(pthread_create(&tsig, NULL, signal_thread, NULL));

  sleep(3);

  MCHECK(pthread_mutex_lock(&mutex));
  printf("# wakeup   = %8lu\n# spurious = %8lu (%2.2f%%)\n", nwakeup, nspurious, (float) nspurious / nwakeup * 100.0);
  running = false;
  MCHECK(pthread_mutex_unlock(&mutex));

  for (int i = 0; i < NTHREADS; i++)
  {
    MCHECK(pthread_detach(tid[i]));
  }
  MCHECK(pthread_detach(tsig));

  return 0;
}
