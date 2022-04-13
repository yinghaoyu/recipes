#include <iostream>
#include <sys/time.h>
using namespace std;

//循环展开只是一部分
//核心原因是f2用比较和加法来减少除法(/=)操作
//f3通过搜索的方式进一步减少了除法操作
int f1(long v)
{
  int result = 0;
  do{
    ++result;
    v /= 10;
  }while(v);
  return result;
}

int f2(long v)
{
  int result = 1;
  for(;;)
  {
    if(v < 10) return result;
    if(v < 100) return result + 1;
    if(v < 1000) return result + 2;
    if(v < 10000) return result + 3;
    v /= 10000U;
    result += 4;
  }
}

int f3(long v)
{
  if(v < 10) return 1;
  if(v < 100) return 2;
  if(v < 1000) return 3;
  if(v < 1000000000000)  // 10^12
  {
    if(v < 100000000)  // 10^8
    {
      if(v < 1000000)  // 10^6
      {
        if(v < 10000) return 4;
        return 5 + (v >= 100000); // 10^5
      }
      return 7 + (v >= 10000000); // 10^7
    }
    if(v < 10000000000)  // 10^10
    {
      return 9 + (v >= 1000000000);  // 10^9
    }
    return 11 + (v >= 100000000000);  // 10^11
  }
  return 12 + f3(v / 1000000000000);  // 10^12
}

#define COUNT 20000  // 取值越大越明显

int main()
{
  srand(100);
  long num[COUNT];
  for(int i = 0; i < COUNT; ++i)
  {
    num[i] = rand();
  }

  struct timeval start, end;
  long sum1 = 0;
  long time1 = 0;
  gettimeofday(&start, NULL);
  for(int i = 0; i < COUNT; ++i)
  {
    sum1 += f1(num[i]);
  }
  gettimeofday(&end, NULL);
  time1 = (end.tv_sec - start.tv_sec) * 1000 * 1000 + end.tv_usec - start.tv_usec;
  
  long sum2 = 0;
  long time2 = 0;
  gettimeofday(&start, NULL);
  for(int i = 0; i < COUNT; ++i)
  {
    sum2 += f2(num[i]);
  }
  gettimeofday(&end, NULL);
  time2 = (end.tv_sec - start.tv_sec) * 1000 * 1000 + end.tv_usec - start.tv_usec;
  
  long sum3 = 0;
  long time3 = 0;
  gettimeofday(&start, NULL);
  for(int i = 0; i < COUNT; ++i)
  {
    sum3 += f3(num[i]);
  }
  gettimeofday(&end, NULL);
  time3 = (end.tv_sec - start.tv_sec) * 1000 * 1000 + end.tv_usec - start.tv_usec;

  cout << "sum1:" << sum1 << "\t sum2:" << sum2 << "\t sum3:" << sum3 << endl;
  cout << "cost1:" << time1 << "\t cost2:" << time2 << "\t cost3:" << time3 << endl;
  return 0;
}
