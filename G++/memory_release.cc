#include <atomic>
#include <cassert>
#include <string>
#include <thread>

std::atomic<std::string *> ptr;
int data;


void producer() {
  auto *p = new std::string("Hello");
  data = 42;
  //store是一个写操作，std::memory_order_release要求在这个写指令完成后，所有的写操作必须也是完成状态，也就是说，
  //编译器看到这条指令后，就不能将data = 42这条指令移动到store操作之后，这便是memory_order_release对内存顺序的要求
  //memory_order_release中的release指的是将修改后（写操作）的结果释放出来，一旦其他线程使用了 memory_order_acquire,就可以观测到
  //上述对内存写入的结果（release也可理解为释放内存的控制权）
  ptr.store(p, std::memory_order_release);//[1]
}

void consumer() {
  std::string *p2;
  //此处等待ptr的store操作 while保证了时间上的同步，也就是会等到ptr写入的那一刻，memory_order_release，memory_order_acquire保证
  //内存上的同步，也就是producer写入的值一定会被consumer读取到
  while (!(p2 = ptr.load(std::memory_order_acquire)))//[3]
    ;
  //如果执行到此处，说明p2是非空的，也就意味着ptr load 到了一个非空的字符串，也就意味着 data = 42的指令已经执行了（memory_order_release保证），
  //且此时data必定等于42 ，p2必定为“Hello”（memory_order_acquire保证）
  assert(*p2 == "Hello");// 绝无问题//[2]
  assert(data == 42);    // 绝无问题
}

int main() {
  std::thread t2(consumer);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  std::thread t1(producer);

  t1.join();
  t2.join();
}
