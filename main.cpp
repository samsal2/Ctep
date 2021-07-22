#include <algorithm>
#include <atomic>
#include <iostream>
#include <type_traits>
#include <utility>

#define DISABLE_EXPLICIT int &...

namespace ctep {

template <typename... Ts> struct pack {};

template <DISABLE_EXPLICIT, typename... Us>
consteval auto make_pack(Us...) noexcept {
  return pack<Us...>{};
}

template <DISABLE_EXPLICIT, typename U, typename... Us>
consteval auto append(pack<Us...>, U) noexcept {
  return pack<Us..., U>{};
}

namespace detail {

// TODO(samuel): Non-recursive method

template <DISABLE_EXPLICIT, typename U, typename... Us>
consteval auto get(pack<U, Us...>, std::integral_constant<size_t, 0>) noexcept {
  return U{};
}

template <DISABLE_EXPLICIT, size_t N, typename U, typename... Us>
consteval auto get(pack<U, Us...>, std::integral_constant<size_t, N>) noexcept {
  return get(pack<Us...>{}, std::integral_constant<size_t, N - 1>{});
}

} // namespace detail

template <size_t N, DISABLE_EXPLICIT, typename U, typename... Us>
consteval auto get(pack<U, Us...>) noexcept {
  return detail::get(pack<U, Us...>{}, std::integral_constant<size_t, N>{});
}

template <size_t N> consteval auto get(pack<>) noexcept = delete;

namespace detail {

template <DISABLE_EXPLICIT, size_t... Is, typename... Us>
consteval auto pop_back(pack<Us...> pack, std::index_sequence<Is...>) noexcept {
  return make_pack(get<Is>(pack)...);
}

} // namespace detail

template <DISABLE_EXPLICIT, typename... Us>
consteval auto pop_back(pack<Us...> pack) noexcept {
  return detail::pop_back(pack, std::make_index_sequence<sizeof...(Us) - 1>{});
}

template <DISABLE_EXPLICIT, typename U, typename... Us>
consteval auto pop_front(pack<U, Us...>) noexcept {
  return pack<Us...>{};
}

template <size_t N> struct string_literal {

  consteval string_literal(char const (&source)[N]) noexcept {
    std::copy(&source[0], &source[N], &source_[0]);
  }

  consteval size_t size() const noexcept { return N; }

  template <size_t Idx> consteval char get() const noexcept {
    return source_[Idx];
  }

  char source_[N];
};

template <char C> struct char_constant : std::integral_constant<char, C> {};

namespace detail {

template <string_literal Literal, DISABLE_EXPLICIT, size_t... Is>
consteval auto make_char_pack(std::index_sequence<Is...>) noexcept {
  return ctep::make_pack(char_constant<Literal.template get<Is>()>{}...);
}

} // namespace detail

template <string_literal Literal> consteval auto make_char_pack() noexcept {
  return detail::make_char_pack<Literal>(
      std::make_index_sequence<Literal.size()>{});
}

namespace detail {

template <typename T>
concept integral = requires {
  requires std::is_integral_v<T>;
};

consteval auto power_10(integral auto number) noexcept {

  decltype(number) base = 1;

  while (number != 1) {
    --number;
    base *= 10;
  }

  return base;
}

template <DISABLE_EXPLICIT, size_t... Is, char... Cs>
consteval auto parse_number(pack<char_constant<Cs>...> pack,
                            std::index_sequence<Is...>) noexcept {
  return (((get<Is>(pack) - '0') * power_10(sizeof...(Is) - Is)) + ...);
}

} // namespace detail

template <DISABLE_EXPLICIT, char... Cs>
consteval auto parse_number(pack<char_constant<Cs>...> pack) noexcept {
  if constexpr (get<0>(pack) == '-') {
    return -detail::parse_number(pop_front(pack),
                                 std::make_index_sequence<sizeof...(Cs) - 1>{});
  }
  return detail::parse_number(pack, std::make_index_sequence<sizeof...(Cs)>{});
}

} // namespace ctep

// NOTE(samuel): Pop the null terminated string
#define CTEP_PARSE_NUMBER_TEST(Num)                                            \
  static_assert(                                                               \
      ctep::parse_number(ctep::pop_back(ctep::make_char_pack<#Num>())) == Num)

int main() {
  CTEP_PARSE_NUMBER_TEST(0);
  CTEP_PARSE_NUMBER_TEST(1);
  CTEP_PARSE_NUMBER_TEST(10);
  CTEP_PARSE_NUMBER_TEST(9223372036854775807);  // signed 64 max
  CTEP_PARSE_NUMBER_TEST(-9223372036854775807); // signed 64 min ?
}