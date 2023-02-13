#include <iostream>
#include <functional>
#include <thread>

using namespace std;

void fun(int& a)
{
  a++;
}

int main()
{
  int a = 1;
  // 因为std::thread的构造函数并不知道fun的参数是引用
  // 所以这里需要显示将a作为引用传参
  std::thread t1(fun, std::ref(a));
  t1.join();
  cout << a << endl;  // 如果不是引用，则a = 1
  return 0;
}
