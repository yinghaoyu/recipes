#include <iostream>
#include <string>
#include <type_traits>

template <typename T>
class Foo {
    T value;

   public:
    template <typename... Args>
        requires std::is_constructible_v<T, Args...>
    constexpr Foo(Args&&... args) : value{std::forward<Args>(args)...} {}
};

template <typename T>
class Bar {
    T value;

   public:
    template <typename... Args>
        requires std::is_constructible_v<T, Args...>
    constexpr Bar(Args&&... args) : value{std::forward<Args>(args)...} {}
};

// 推导指引
template <typename T>
Bar(T) -> Bar<std::decay_t<T>>;

int main() {
    //Foo foo{"hello"};  // 这句会报错
    Bar bar{"hello"};
}

