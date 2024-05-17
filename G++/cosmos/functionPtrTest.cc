#include <iostream>

void func(void) {
  //...
}

struct Foo {
  void operator()(void) {
    //...
  }
};

struct Bar {
  using fr_t = void (*)(void);

  static void func(void) {
    //...
  }

  // 想一想这里重载了什么? 答：什么都没重载，这里是用户自定义转换函数
  // https://zh.cppreference.com/w/cpp/language/cast_operator
  operator fr_t(void) { return func; }
};

struct A {
  int a_;

  void mem_func(void) {
    //...
  }
};

int main(void) {
  void (*func_ptr)(void) = &func;  // 函数指针
  func_ptr();

  Foo foo;  // 仿函数
  foo();

  Bar bar;  // 可被转换为函数指针的类对象
  bar();

  void (A::*mem_func_ptr)(void) = &A::mem_func;  // 类成员函数指针
  int A::*mem_obj_ptr = &A::a_;                  // 类成员指针

  A aa;
  (aa.*mem_func_ptr)();
  aa.*mem_obj_ptr = 123;

  return 0;
}
