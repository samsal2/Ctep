#ifndef CTEP_STRING_LITERAL_HPP
#define CTEP_STRING_LITERAL_HPP

#include <algorithm>

namespace ctep {

template <size_t N> struct string_literal {

  consteval string_literal(char const (&source)[N]) noexcept {
    std::copy(&source[0], &source[N], &source_[0]);
  }

  char source_[N];
};

} // namespace ctep

#endif
