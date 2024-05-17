#include <algorithm>
#include <functional>
#include <iostream>
#include <type_traits>

template <typename T>
class Task;

template <typename R, typename... Args>
class Task<R(Args...)> {
 public:
  Task(std::function<R(Args...)>&& f) : m_fn(std::move(f)) {}

  Task(std::function<R(Args...)>& f) : m_fn(f) {}

  template <typename... NewArgs>
  R Run(NewArgs&&... args) {
    return m_fn(std::forward<NewArgs>(args)...);
  }

  template <typename F>
  auto Then(F&& f) -> Task<typename std::result_of<F(R)>::type(Args...)> {
    // NOTE: 把之前的返回值 R ，用作当前函数 F 的入参
    using return_type = typename std::result_of<F(R)>::type;

    auto func = std::move(m_fn);

    return Task<return_type(Args...)>([func, f](Args&&... args) {
      return f(func(std::forward<Args>(args)...));
    });
  }

 private:
  std::function<R(Args...)> m_fn;
};

void TestTask() {
  // 为何这里用 = 会提示转换不匹配?
  // 答：lambda 转到 std::function 算一次转换，不能直接从 lambda 转换到 Task
  // https://zh.cppreference.com/w/cpp/language/copy_initialization
  Task<int(int)> task = [](int i) { return i; };
  auto result = task.Then([](int i) { return i + 1; })
                    .Then([](int i) { return i + 2; })
                    .Then([](int i) { return i + 3; })
                    .Run(1);
  std::cout << result << std::endl;  // 7
}

int main(void) {
  TestTask();

  return 0;
}
