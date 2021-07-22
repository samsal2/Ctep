#include <algorithm>
#include <atomic>
#include <iostream>
#include <type_traits>
#include <utility>

#define DISABLE_EXPLICIT int &...

namespace Ctep {

template <typename... Ts> struct Pack {};

template <DISABLE_EXPLICIT, typename... Us>
consteval auto pack(Us...) noexcept {
  return Pack<Us...>{};
}

template <DISABLE_EXPLICIT, typename U, typename... Us>
consteval auto append(Pack<Us...>, U) noexcept {
  return Pack<Us..., U>{};
}

namespace Detail {

// TODO(samuel): Non-recursive method

template <DISABLE_EXPLICIT, typename U, typename... Us>
consteval auto get(Pack<U, Us...>, std::integral_constant<size_t, 0>) noexcept {
  return U{};
}

template <DISABLE_EXPLICIT, size_t N, typename U, typename... Us>
consteval auto get(Pack<U, Us...>, std::integral_constant<size_t, N>) noexcept {
  return get(Pack<Us...>{}, std::integral_constant<size_t, N - 1>{});
}

} // namespace Detail

template <size_t N, DISABLE_EXPLICIT, typename U, typename... Us>
consteval auto get(Pack<U, Us...>) noexcept {
  return Detail::get(Pack<U, Us...>{}, std::integral_constant<size_t, N>{});
}

template <size_t N> consteval auto get(Pack<>) noexcept = delete;

namespace Detail {

template <DISABLE_EXPLICIT, size_t... Is, typename... Us>
consteval auto pop(Pack<Us...> Pack, std::index_sequence<Is...>) noexcept {
  return pack(get<Is>(Pack)...);
}

} // namespace Detail

template <DISABLE_EXPLICIT, typename... Us>
consteval auto pop(Pack<Us...> Pack) noexcept {
  return Detail::pop(Pack, std::make_index_sequence<sizeof...(Us) - 1>{});
}

template <DISABLE_EXPLICIT, typename U, typename... Us>
consteval auto popFront(Pack<U, Us...>) noexcept {
  return Pack<Us...>{};
}

template <size_t N> struct StringLiteral {

  consteval StringLiteral(char const (&String)[N]) noexcept {
    std::copy(&String[0], &String[N], &Source[0]);
  }

  consteval size_t size() const noexcept { return N; }

  template <size_t Idx> consteval char get() const noexcept {
    return Source[Idx];
  }

  char Source[N];
};

template <char C> struct Char : std::integral_constant<char, C> {};

namespace Detail {

template <StringLiteral Literal, DISABLE_EXPLICIT, size_t... Is>
consteval auto makeCharPack(std::index_sequence<Is...>) noexcept {
  return Ctep::pack(Char<Literal.template get<Is>()>{}...);
}

} // namespace Detail

template <StringLiteral Literal> consteval auto makeCharPack() noexcept {
  return Detail::makeCharPack<Literal>(
      std::make_index_sequence<Literal.size()>{});
}

namespace Detail {

template <typename T>
concept Integral = requires {
  requires std::is_integral_v<T>;
};

consteval auto power10(Integral auto Number) noexcept {

  decltype(Number) Base = 1;

  while (Number != 1) {
    --Number;
    Base *= 10;
  }

  return Base;
}

template <DISABLE_EXPLICIT, size_t... Is, char... Cs>
consteval auto parseNumber(Pack<Char<Cs>...> Pack,
                           std::index_sequence<Is...>) noexcept {
  return (((get<Is>(Pack) - '0') * power10(sizeof...(Is) - Is)) + ...);
}

} // namespace Detail

template <DISABLE_EXPLICIT, char... Cs>
consteval auto parseNumber(Pack<Char<Cs>...> Pack) noexcept {
  if constexpr (get<0>(Pack) == '-') {
    return -Detail::parseNumber(popFront(Pack),
                                std::make_index_sequence<sizeof...(Cs) - 1>{});
  }
  return Detail::parseNumber(Pack, std::make_index_sequence<sizeof...(Cs)>{});
}

} // namespace Ctep

#define CTEP_PARSE_NUMBER_TEST(Num)                                            \
  static_assert(Ctep::parseNumber(Ctep::pop(Ctep::makeCharPack<#Num>())) == Num)

int main() {
  CTEP_PARSE_NUMBER_TEST(0);
  CTEP_PARSE_NUMBER_TEST(1);
  CTEP_PARSE_NUMBER_TEST(10);
  CTEP_PARSE_NUMBER_TEST(9223372036854775807);  // signed 64 max
  CTEP_PARSE_NUMBER_TEST(-9223372036854775807); // signed 64 min ?
}