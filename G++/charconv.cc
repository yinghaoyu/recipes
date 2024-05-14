#include <charconv>
#include <iostream>
#include <sstream>

using namespace std;

// NOTE: 将int 转换成16 进制的hex string

void f1() {
  int val = 42;
  std::ostringstream ss;
  ss << std::hex << val;
  std::string result = ss.str();
  std::cout << "0x" << result << "\n";  // 0x2a
}

// c++17 巧用 charconv
// 方法2 比 方法1 快不少
void f2() {
  int val = 42;
  char buf[20];
  auto [ptr, ec] = std::to_chars(std::begin(buf), std::end(buf), val, 16);
  std::string_view str{buf, size_t(std::distance(buf, ptr))};
  std::cout << "0x" << str << "\n";
}
