#include <iostream>
#include <pthread.h>
#include <sys/time.h>

using namespace std;

#define N 500000

int a[50000];
int b[50000];

void *fun1(void *) {
  timeval start, end;
  gettimeofday(&start, NULL);
  for (int i = 0; i < N; i++) {
    a[0]++;
  }
  gettimeofday(&end, NULL);
  printf("fun1, time=%d\n", (int)(end.tv_sec - start.tv_sec) * 1000000 +
                                (int)(end.tv_usec - start.tv_usec));
  return NULL;
}

void *fun2(void *) {
  timeval start, end;
  gettimeofday(&start, NULL);
  for (int i = 0; i < N; i++) {
    a[1]++;
  }
  gettimeofday(&end, NULL);
  printf("fun2, time=%d\n", (int)(end.tv_sec - start.tv_sec) * 1000000 +
                                (int)(end.tv_usec - start.tv_usec));
  return NULL;
}

void *fun3(void *) {
  timeval start, end;
  gettimeofday(&start, NULL);
  for (int i = 0; i < N; i++) {
    b[0]++;
  }
  gettimeofday(&end, NULL);
  printf("fun3, time=%d\n", (int)(end.tv_sec - start.tv_sec) * 1000000 +
                                (int)(end.tv_usec - start.tv_usec));
  return NULL;
}

int main() {
  pthread_t t1;
  pthread_t t2;
  pthread_t t3;
  pthread_t t4;
  pthread_t t5;
  pthread_t t6;
  // true sharing
  pthread_create(&t1, NULL, fun1, NULL);
  pthread_create(&t2, NULL, fun1, NULL);
  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  // false sharing
  pthread_create(&t3, NULL, fun1, NULL);
  pthread_create(&t4, NULL, fun2, NULL);
  pthread_join(t3, NULL);
  pthread_join(t4, NULL);
  // no sharing
  pthread_create(&t5, NULL, fun1, NULL);
  pthread_create(&t6, NULL, fun3, NULL);
  pthread_join(t5, NULL);
  pthread_join(t6, NULL);
  return 0;
}
