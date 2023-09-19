#ifndef __THREAD_LOCAL_H__
#define __THREAD_LOCAL_H__

#include <windows.h>

namespace base
{
template <typename T>
class ThreadLocal
{
 public:
  ThreadLocal()
  {
    key_ = TlsAlloc();
    if (key_ == TLS_OUT_OF_INDEXES)
    {
      throw std::runtime_error("ThreadLocal - TlsAlloc failed");
    }
  }

  ~ThreadLocal() { TlsFree(key_); }

  T &value()
  {
    T *ptr = static_cast<T *>(TlsGetValue(key_));
    if (ptr == NULL)
    {
      ptr = new T();
      TlsSetValue(key_, ptr);
    }
    return *ptr;
  }

 private:
  DWORD key_;
};
}  // namespace base

/* test
// 示例用的数据类型
struct MyData {
    int threadId;
};

// 在线程中使用 ThreadLocal
void ThreadFunction(base::ThreadLocal<MyData>& tl) {
    MyData& data = tl.value();
    data.threadId = GetCurrentThreadId();

    // 模拟一些工作
    Sleep(1000);

    // 打印线程ID和线程本地数据
    std::cout << "Thread ID: " << GetCurrentThreadId() << ", ThreadLocal Data: " << data.threadId << std::endl;
}

int main() {
    const int numThreads = 3;
    HANDLE threads[numThreads];
    base::ThreadLocal<MyData> threadLocal;

    for (int i = 0; i < numThreads; ++i) {
        threads[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFunction, &threadLocal, 0, NULL);
    }

    WaitForMultipleObjects(numThreads, threads, TRUE, INFINITE);

    for (int i = 0; i < numThreads; ++i) {
        CloseHandle(threads[i]);
    }

    return 0;
}*/

#endif