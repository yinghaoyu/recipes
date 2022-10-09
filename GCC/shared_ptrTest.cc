#include <iostream>
#include <memory>

using namespace std;

class A : public std::enable_shared_from_this<A>
{
 public:
  A() { cout << "A ctor" << endl; }
  ~A() { cout << "A dtor" << endl; }
  void func()
  {
    // 循环引用
    // 这里出现矛盾：必须销毁 A 才能销毁其成员变量 self_，而销毁 A 前必须先销毁 self_
    // 所以，一个资源的生命周期可以交给一个智能指针对象来管理，但是该智能指针的生命周期不可以再交给该资源来管理
    self_ = shared_from_this();
  }

 public:
  std::shared_ptr<A> self_;
};

int main()
{
  std::shared_ptr<A> ptr(new A());
  ptr->func();
  return 0;
}
