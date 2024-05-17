#include <iostream>
#include <tuple>

template <class Tuple, std::size_t N>
struct TuplePrinter {
  static void print(const Tuple& t) {
    TuplePrinter<Tuple, N - 1>::print(t);
    std::cout << ", " << std::get<N - 1>(t);
  }
};

template <class Tuple>
struct TuplePrinter<Tuple, 1> {
  static void print(const Tuple& t) { std::cout << std::get<0>(t); }
};

// 模版参数
template <class... Args>
void PrintTuple(const std::tuple<Args...>& t) {
  std::cout << "(";
  TuplePrinter<decltype(t), sizeof...(Args)>::print(t);
  std::cout << ")";
}

int main(void) {
  std::tuple<int, short, double, char> tp = std::make_tuple(1, 2, 3, 'a');
  PrintTuple(tp);  // print : (1, 2, 3, a)

  return 0;
}
