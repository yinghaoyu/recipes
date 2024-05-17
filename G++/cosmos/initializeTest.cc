int i_arr[3] = {1, 2, 3};

struct A {
  int x;
  struct B {
    int i;
    int j;
  } b;
} a = {1, {2, 3}};  // POD

int i = 0;

class Foo {
 public:
  Foo(int) {}
} foo = 123;

int j(0);

Foo bar(123);

int main(void) { return 0; }
