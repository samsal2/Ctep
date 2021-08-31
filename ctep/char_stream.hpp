#ifndef CTEP_MISC_HPP
#define CTEP_MISC_HPP

#include "ctep/pack.hpp"
#include "ctep/string_literal.hpp"

#include <utility>

namespace ctep {

template <char C> struct char_constant : std::integral_constant<char, C> {};

template <string_literal Literal> struct literal_constant {};

template <string_literal Literal>
static constexpr inline auto use_literal = literal_constant<Literal>{};

namespace detail {

template <DISABLE_EXPLICIT, string_literal Literal, size_t... Is>
consteval auto make_char_stream(literal_constant<Literal>,
                                std::index_sequence<Is...>) noexcept {
  return make_pack(char_constant<Literal.source_[Is]>{}...);
}

} // namespace detail

template <DISABLE_EXPLICIT, string_literal Literal>
consteval auto make_char_stream(literal_constant<Literal> literal) noexcept {
  return detail::make_char_stream(
      literal, std::make_index_sequence<sizeof(Literal.source_)>{});
}

} // namespace ctep

#endif
