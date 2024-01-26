#include <coroutine>
#include <iostream>
#include <thread>

// https://blog.csdn.net/qq_31442743/article/details/131087944

struct MyAwaiter {
  bool await_ready() {
    std::cout << "current, no data to read" << std::endl;
    return false;
  }

  void await_resume() { std::cout << "get data to read" << std::endl; }

  void await_suspend(std::coroutine_handle<> h) {
    std::cout << "suspended self, wait data to read" << std::endl;
  }
};

template <typename T>
struct Task_ret {
  struct promise_type;
  using handle_type = std::coroutine_handle<promise_type>;
  handle_type task_handle_;
  bool task_done_{false};

  Task_ret(handle_type h) : task_handle_(h) {}

  Task_ret(const Task_ret&) = delete;
  Task_ret(Task_ret&& s) : task_handle_(s.task_handle_) {
    s.task_handle_ = nullptr;
  }
  ~Task_ret() {
    if (task_handle_) {
      std::cout << "destroy" << std::endl;
      task_handle_.destroy();
    }
  }

  void resume() {
    task_handle_.resume();
    task_done_ = task_handle_.done();
  }

  void start() { task_handle_.resume(); }

  bool done() const { return task_done_; }

  T get() { return task_handle_.promise().return_data_; }

  struct promise_type {
    promise_type() = default;
    ~promise_type() = default;

    auto get_return_object() noexcept {
      return Task_ret<T>{handle_type::from_promise(*this)};
    }

    auto initial_suspend() noexcept { return std::suspend_always{}; }

    auto final_suspend() noexcept {
      std::cout << "final suspend" << std::endl;
      return std::suspend_always{};
    }

    void unhandled_exception() { std::exit(1); }

    void return_value(T v) {
      std::cout << "return_value invoked." << std::endl;
      return_data_ = v;
      return;
    }

    auto yield_value(T v) {
      std::cout << "yield_value invoked." << std::endl;
      return_data_ = v;
      return std::suspend_always{};
    }

    T return_data_;
  };
};

template <typename T>
Task_ret<T> ReadCoroutineFunc() {
  std::cout << "Coroutine co_await std::suspend_never" << std::endl;
  co_await std::suspend_never{};
  std::cout << "Coroutine co_await std::suspend_always" << std::endl;
  co_await std::suspend_always{};
  std::cout << "after Coroutine co_await std::suspend_always" << std::endl;
  co_await std::suspend_always{};
  std::cout << "after Coroutine co_await std::suspend_always" << std::endl;

  co_await MyAwaiter{};

  co_yield 1;

  co_yield 2;

  co_return 3;
}

int main() {
  Task_ret<int> task_ret = ReadCoroutineFunc<int>();
  std::cout << "start to coroutine" << std::endl;
  task_ret.start();
  task_ret.resume();
  task_ret.resume();
  std::cout << "before while" << std::endl;
  while (!task_ret.done()) {
    task_ret.resume();
    std::cout << "get yield value: " << task_ret.get() << std::endl;
  }

  std::cout << "end of code" << std::endl;
}
