#include <coroutine>
#include <iostream>
#include <thread>

// https://blog.csdn.net/qq_31442743/article/details/131087944

std::coroutine_handle<> handle;

struct ReadAwaiter {
  bool await_ready() {  // 4
    std::cout << "current, no data to read" << std::endl;
    // return fasle means no ready, goto await_suspend
    return false;
  }

  void await_resume()  // 7
  {
    std::cout << "get data to read" << std::endl;
  }

  void await_suspend(std::coroutine_handle<> h) {  // 5
    std::cout << "suspended self, wait data to read" << std::endl;
    handle = h;
    // coroutine suspend now
  }
};

struct Promise {
  struct promise_type {
    auto get_return_object() noexcept {  // 1
      std::cout << "get return object" << std::endl;
      return Promise();
    }

    auto initial_suspend() noexcept {  // 2
      std::cout << "initial suspend, return never" << std::endl;
      return std::suspend_never{};
    }

    auto final_suspend() noexcept  // 8
    {
      std::cout << "final suspend, return never" << std::endl;
      return std::suspend_never{};
    }

    void unhandled_exception() {
      std::cout << "unhandle exception" << std::endl;
      std::terminate();
    }

    void return_void()  // 9
    {
      std::cout << "return void" << std::endl;
      return;
    }
  };
};

Promise ReadCoroutineFunc() {
  co_await ReadAwaiter();  // 3 triggle await_ready
  // implicit co_return;
}

int main() {
  ReadCoroutineFunc();

  std::this_thread::sleep_for(std::chrono::seconds(1));
  std::cout << "sleep 1s and then read data" << std::endl;
  handle.resume();  // 6 triggle await_resume
}
