#include <array>
#include <iostream>
#include <string>

using namespace std;

// 可变参数模版 Varidic Template

namespace case1 {
void print() {}

template <typename T, typename... Types>
void print(T first, Types... args) {
  cout << first << endl;
  print(args...);
}
}  // namespace case1

namespace case2 {

void print() {}

template <typename T, typename... Types>
void print(T first, Types... args) {
  cout << first << endl;
  if constexpr (sizeof...(args) > 0) {  // c++17 compile-time if
    print(args...);
  }
}
}  // namespace case2

namespace case3 {
template <typename... T>
auto foldSum(T... s) {
  return (... + s);
}

template <typename... Types>
void print(Types const&... args) {
  (cout << ... << args) << endl;
}
}  // namespace case3

namespace case4 {
template <typename T, typename... Args>
void print(T first, Args... args) {
  std::cout << first;
  auto printWhiteSpace = [](const auto arg) { std::cout << " " << arg; };
  (..., printWhiteSpace(args));
}
}  // namespace case4

namespace case5 {
template <typename... Types>
void print(Types const&... args) {
  (cout << ... << args) << endl;
}

template <typename C, typename... Idx>
void printElems(C const& coll, Idx... idx) {
  print(coll[idx]...);
}

}  // namespace case5

// namespace std {
// template <typename T, typename... U>
// array(T, U...)
//     -> array<enable_if_t<(is_same_v<T, U> && ...), T>, (1 + sizeof...(U))>;
// }

int main() {
  string s = "world";
  // case1::print(7.5, "hello", s);
  // case2::print(7.5, "hello", s);
  // case3::print(7.5, "hello", s);
  // case4::print(7.5, "hello", s);
  // vector<string> coll = {"good", "times", "say", "bye"};
  // case5::printElems(coll, 2, 0, 3);
  std::array arr{1, 2, 3, 4, 5};
  return 0;
}
