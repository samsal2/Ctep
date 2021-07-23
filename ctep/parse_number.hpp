#ifndef CTEP_PARSE_NUMBER_HPP
#define CTEP_PARSE_NUMBER_HPP

#include "ctep/char_stream.hpp"
#include <type_traits>
#include <utility>

namespace ctep {

namespace detail {

template <typename T>
concept integral = requires {
  requires std::is_integral_v<T>;
};

consteval auto base10(integral auto number) noexcept {
  decltype(number) base = 1;

  while (number > 1) {
    --number;
    base *= 10;
  }

  return base;
}

template <DISABLE_EXPLICIT, size_t... Is, char... Cs>
consteval auto parse_number(pack<char_constant<Cs>...> pack,
                            std::index_sequence<Is...>) noexcept {
  return (((get<Is>(pack) - '0') * base10(sizeof...(Is) - Is)) + ...);
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

#endif
