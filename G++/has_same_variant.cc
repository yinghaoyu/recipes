#include <type_traits>
#include <variant>

// 编译时检查 std::variant 是否有相同类型

namespace case1 {

// NOTE: 这种写法只检查了第一个类型，而没有检查后面的类型
// 递归的终点
template <typename T>
struct has_same_variant_type : std::false_type {};

// 多个参数递归
template <typename T, typename... Us>
struct has_same_variant_type<std::variant<T, Us...>>
    : std::disjunction<std::is_same<T, Us>...> {};

template <typename T>
constexpr bool has_same_variant_type_v = has_same_variant_type<T>::value;

}  // namespace case1

namespace case2 {
// 正确写法
template <typename T>
struct has_same_variant_type : std::false_type {};

template <typename T, typename... Us>
struct has_same_variant_type<std::variant<T, Us...>> {
  template <typename M, typename... N>
  using Helper = std::disjunction<std::is_same<M, N>...>;

  // template <bool B, typename T, typename F>
  // using conditional_t = typename std::conditional<B, T, F>::type;
  // B 是一个编译期常量布尔值，用于指定条件
  // T 是一个类型，表示条件为真时的结果
  // F 是另一个类型，表示条件为假时的结果

  // 通过std::conditional_t去递归调用has_same_variant_type，直到遇到重复类型为止
  static constexpr bool value =
      std::conditional_t<Helper<T, Us...>::value, std::true_type,
                         has_same_variant_type<std::variant<Us...>>>::value;
};

template <typename T>
constexpr bool has_same_variant_type_v = has_same_variant_type<T>::value;
}  // namespace case2

int main() {
  std::variant<int, bool> v1;
  std::variant<int, bool, int> v2;
  std::variant<int, bool, bool> v3;
  static_assert(!case1::has_same_variant_type_v<decltype(v1)>);
  static_assert(case1::has_same_variant_type_v<decltype(v2)>);
  static_assert(case1::has_same_variant_type_v<decltype(v3)>);  // fail!

  static_assert(!case2::has_same_variant_type_v<decltype(v1)>);
  static_assert(case2::has_same_variant_type_v<decltype(v2)>);
  static_assert(case2::has_same_variant_type_v<decltype(v3)>);  // right
  return 0;
}
