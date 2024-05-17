#include <iostream>
#include <tuple>

// TODO: 理解

// 接受一个或多个整数作为参数模版
template <int...>
struct IndexTuple {};

// 生成从 0 到 N-1 的索引序列，它接受一个整数 N
// 作为参数，并使用递归来生成索引序列
template <int N, int... Indexs>
struct MakeIndexes : MakeIndexes<N - 1, N - 1, Indexs...> {};

// MakeIndexes 模板的递归终止条件，当 N 等于 0 时，
// 停止递归并返回一个 IndexTuple，其中包含了之前生成的索引序列
template <int... Indexs>
struct MakeIndexes<0, Indexs...> {
  typedef IndexTuple<Indexs...> type;
};

// 前置声明
template <int I, typename IndexTuple, typename... Types>
struct make_indexes_reverse_impl;

// declare
template <int I, int... Indexes, typename T, typename... Types>
struct make_indexes_reverse_impl<I, IndexTuple<Indexes...>, T, Types...> {
  using type =
      typename make_indexes_reverse_impl<I - 1, IndexTuple<Indexes..., I - 1>,
                                         Types...>::type;
};

// terminate
template <int I, int... Indexes>
struct make_indexes_reverse_impl<I, IndexTuple<Indexes...>> {
  using type = IndexTuple<Indexes...>;
};

// type trait
template <typename... Types>
struct make_reverse_indexes
    : make_indexes_reverse_impl<sizeof...(Types), IndexTuple<>, Types...> {};

// ·´×ª
template <class... Args, int... Indexes>
auto reverse_impl(std::tuple<Args...>&& tup, IndexTuple<Indexes...>&&)
    -> decltype(std::make_tuple(
        std::get<Indexes>(std::forward<std::tuple<Args...>>(tup))...)) {
  return std::make_tuple(
      std::get<Indexes>(std::forward<std::tuple<Args...>>(tup))...);
}

template <class... Args>
auto Reverse(std::tuple<Args...>&& tup)
    -> decltype(reverse_impl(std::forward<std::tuple<Args...>>(tup),
                             typename make_reverse_indexes<Args...>::type())) {
  return reverse_impl(std::forward<std::tuple<Args...>>(tup),
                      typename make_reverse_indexes<Args...>::type());
}

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

template <class... Args>
void PrintTuple(const std::tuple<Args...>& t) {
  std::cout << "(";
  TuplePrinter<decltype(t), sizeof...(Args)>::print(t);
  std::cout << ")\n";
}

void TupleReverse() {
  auto tp1 = std::make_tuple<int, short, double, char>(1, 2, 2.5, 'a');
  auto tp2 = Reverse(std::make_tuple<int, short, double, char>(1, 2, 2.5, 'a'));

  PrintTuple(tp1);
  PrintTuple(tp2);
}

int main(void) {
  TupleReverse();

  return 0;
}
