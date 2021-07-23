#include "ctep/char_stream.hpp"
#include "ctep/pack.hpp"
#include "ctep/parse_number.hpp"
#include "ctep/string_literal.hpp"

#define CTEP_TEST_PARSE_NUMBER(Number)                                         \
  do {                                                                         \
                                                                               \
    auto stream_ = ctep::make_char_stream(ctep::use_literal<#Number>);         \
    static_assert(ctep::parse_number(ctep::pop_back(stream_)) == Number);      \
  } while (0)

int main() {
  CTEP_TEST_PARSE_NUMBER(1);
  CTEP_TEST_PARSE_NUMBER(2);
  CTEP_TEST_PARSE_NUMBER(123456);
  CTEP_TEST_PARSE_NUMBER(9223372036854775807);  // max int 64
  CTEP_TEST_PARSE_NUMBER(-9223372036854775807); // min int 64

  auto stream = ctep::make_char_stream(ctep::use_literal<"421">);
  constexpr auto num = ctep::parse_number(ctep::pop_back(stream));
  static_assert(num == 421);
}