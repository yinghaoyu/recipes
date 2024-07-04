#include <array>
#include <string_view>
#include "frozen/string.h"
#include "frozen/unordered_map.h"

// 编译期如何反射枚举变量

//@sa
// https://bbbgan.github.io//2023/07/01/%E5%A6%82%E4%BD%95%E5%8F%8D%E5%B0%84%E6%9E%9A%E4%B8%BE%E5%8F%98%E9%87%8F/
namespace iguana {

// clang 17需要使用
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wenum-constexpr-conversion"

template <typename E, E V>
constexpr std::string_view get_raw_name() {
#ifdef _MSC_VER
  return __FUNCSIG__;
#else
  return __PRETTY_FUNCTION__;
#endif
}

template <typename E, E V>
constexpr inline std::pair<bool, std::string_view> try_get_enum_name() {
  constexpr std::string_view sample_raw_name = get_raw_name<int, 5>();
  constexpr size_t pos = sample_raw_name.find("5");
  constexpr std::string_view enum_raw_name = get_raw_name<E, V>();
  constexpr auto enum_end = enum_raw_name.rfind(&sample_raw_name[pos + 1]);
#ifdef _MSC_VER
  constexpr auto enum_begin = enum_raw_name.rfind(',', enum_end) + 1;
#else
  constexpr auto enum_begin = enum_raw_name.rfind(' ', enum_end) + 1;
#endif
  constexpr std::string_view res =
      enum_raw_name.substr(enum_begin, enum_end - enum_begin);

  constexpr size_t pos_brackets = res.find(')');

  size_t pos_colon = res.find("::");
  return {pos_brackets == std::string_view::npos,
          res.substr(pos_colon == std::string_view::npos ? 0 : pos_colon + 2)};
}

template <typename E, std::size_t I>
constexpr auto get_enum_name_helper() {
  return try_get_enum_name<E, static_cast<E>(I)>();
}

template <typename E, std::size_t... Is>
constexpr inline auto get_enum_arr(const std::index_sequence<Is...>&) {
  constexpr std::size_t N = sizeof...(Is);
  std::array<std::string_view, N> enum_names = {};
  std::array<E, N> enum_values = {};
  std::size_t num = 0;
  (([&]() {
     constexpr auto res = get_enum_name_helper<E, Is>();
     if constexpr (res.first) {
       enum_names[num] = res.second;
       enum_values[num] = static_cast<E>(Is);
       ++num;
     }
   })(),
   ...);
  return std::make_tuple(num, enum_values, enum_names);
}

template <typename E, size_t N, size_t... Is>
constexpr inline auto get_enum_to_str_map(
    const std::array<std::string_view, N>& enum_names,
    const std::array<E, N>& enum_values, const std::index_sequence<Is...>&) {
  return frozen::unordered_map<E, frozen::string, sizeof...(Is)>{
      {enum_values[Is], enum_names[Is]}...};
}

template <typename E, size_t N, size_t... Is>
constexpr inline auto get_str_to_enum_map(
    const std::array<std::string_view, N>& enum_names,
    const std::array<E, N>& enum_values, const std::index_sequence<Is...>&) {
  return frozen::unordered_map<frozen::string, E, sizeof...(Is)>{
      {enum_names[Is], enum_values[Is]}...};
}

template <bool str_to_enum, typename E, size_t N = 50>
constexpr inline auto get_enum_map() {
  constexpr auto t = get_enum_arr<E>(std::make_index_sequence<N>());

  if constexpr (str_to_enum) {
    return get_str_to_enum_map<E, N>(
        std::get<2>(t), std::get<1>(t),
        std::make_index_sequence<std::get<0>(t)>{});
  } else {
    return get_enum_to_str_map<E, N>(
        std::get<2>(t), std::get<1>(t),
        std::make_index_sequence<std::get<0>(t)>{});
  }
}
#pragma clang diagnostic pop

}  // namespace iguana

#include <iostream>

using namespace iguana;

enum class Fruit {
  APPLE = 5,
  BANANA = 4,
};

int main() {
  static constexpr auto str_to_enum = get_enum_map<true, Fruit>();
  static constexpr auto enum_to_str = get_enum_map<false, Fruit>();
  std::cout << static_cast<int>(str_to_enum.find("APPLE")->second) << std::endl;
  auto str = enum_to_str.find(Fruit::APPLE)->second;
  std::cout << std::string_view(str.data(), str.size()) << std::endl;
}
