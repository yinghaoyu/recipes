#include <array>
#include <iostream>
#include <mutex>

using namespace std;

class X
{
  public:
    std::mutex m;
    int number;
    bool operator==(const X & other)
    {
      return this == &other;
    }
    static void swap(X& l, X&r)
    {
      if(l == r)
      {
        return;
      }
      // 同时上两把锁，避免死锁风险
      std::lock(l.m, r.m);
      // std::adopt_lock表示这个当前mutex已经被lock
      // 只需要unlock操作
      std::lock_guard<std::mutex> guard_a(l.m, std::adopt_lock);
      std::lock_guard<std::mutex> guard_b(r.m, std::adopt_lock);
      std::swap(l.number, r.number);
    }
};

int main()
{
  X a;
  X b;
  X::swap(a, b);
  return 0;
}
