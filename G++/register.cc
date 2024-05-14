#include <functional>
#include <iostream>
#include <map>
#include <string>

// NOTE: 在main之前实现自动注册

// 定义在 global.h
#define MACRO_CONCAT(A, B) MACRO_CONCAT1(A, B)
#define MACRO_CONCAT1(A, B) A##_##B

#ifdef _MSC_VER
#define IGUANA_UNIQUE_VARIABLE(str) MACRO_CONCAT(str, __COUNTER__)
#else
#define IGUANA_UNIQUE_VARIABLE(str) MACRO_CONCAT(str, __LINE__)
#endif

// 如果 g_map 定义在头文件，使用 inline 消除变量重定义
// 变量名由唯一的文件名和行号组成，这个名字肯定是全局唯一的
#define REGISTER_FUNC(file, key, fn) \
  inline auto IGUANA_UNIQUE_VARIABLE(file) = register_func(key, fn);

std::map<std::string, std::function<int(int)>> g_map;
int register_func(std::string key, std::function<int(int)> f) {
  g_map.emplace(std::move(key), std::move(f));
  return 0;
}

// 定义在 main.cc
REGISTER_FUNC(main, "d", [](int a) { return a; });
REGISTER_FUNC(main, "e", [](int a) { return a; });

int main() { std::cout << g_map.size() << "\n"; }
