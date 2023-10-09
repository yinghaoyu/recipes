#include <functional>
#include <iostream>

using namespace std;

template <typename T>
class A : private T
{
 public:
  A(T &&f) : T(std::move(f)) { std::cout << "A ctor" << endl; }
  ~A()
  {
    // 利用函数对象回调
    (*this)();
    std::cout << "A dtor~" << endl;
  }
};

int main()
{
  A<std::function<void(void)>>([] { cout << "callback" << endl; });
  return 0;
}
