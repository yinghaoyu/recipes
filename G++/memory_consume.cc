// @sa https://www.cnblogs.com/xdblog/p/memory_order.html
// @sa https://lancern.xyz/2021/07/07/cpp-memory-order/

// consume 仅对原子读操作有效

// acquire 仅对原子读操作有效
// acquire 与 consume 唯一的区别是 acquire 要求
// 线程 B 能够看到线程 A 中在 release 操作之前的所有写操作，
// 而不仅仅是与写原子变量具有数据依赖关系的写操作。

// release 仅对原子写操作有效
// release 操作通常是与 consume 或 acquire 操作配对的。
// 当某个线程执行 release 操作后，其他对同一原子变量执行 consume 或 acquire 操作的线程
// 将以相应的粒度看到该线程 release 操作之前的写操作

#include <thread>
#include <atomic>
#include <cassert>
#include <string>
 
std::atomic<std::string*> ptr;
int data;
 
void producer()
{
    std::string* p  = new std::string("Hello");
    data = 42;
    ptr.store(p, std::memory_order_release);
}
 
void consumer()
{
    std::string* p2;
    while (!(p2 = ptr.load(std::memory_order_consume)))
        ;
    assert(*p2 == "Hello"); // 绝无出错： *p2 从 ptr 携带依赖
    assert(data == 42); // 可能也可能不会出错： data 不从 ptr 携带依赖
}
 
int main()
{
    std::thread t1(producer);
    std::thread t2(consumer);
    t1.join(); t2.join();
}
