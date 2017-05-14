#pragma once

#include <algorithm>
#include <utility>

#include "dispatch/string_literal.hpp"
#include "dispatch/utilities.hpp"

namespace sl = jk::string_literal;

// Disadvantages:
// Runtime complexity is O(n) where n is the length of the longest string key
// collision on certain anagrams

template<typename... Strings>
struct simple_string_hash {
  static constexpr unsigned MaxLength = max_string_length(Strings{}...);

  auto operator()(const char* keyword) const {
    unsigned total = strlen(keyword);
    auto max = std::min(total, MaxLength);
    for (unsigned i = 0; i < max; ++i) {
      total += keyword[i] + i;
    }
    return total;
  }

  template<typename StringLiteral, size_t ...I>
  static constexpr auto compute_helper(StringLiteral&&, const std::index_sequence<I...>&) {
    using S = std::decay_t<StringLiteral>;
    return ((S::value().data()[I] + I) + ...);
  }

  template<typename StringLiteral>
  static constexpr auto hash(StringLiteral&& literal) {
    using S = std::decay_t<StringLiteral>;
    return compute_helper(literal, std::make_index_sequence<S::value().size()>{}) + S::value().size();
  }
};

template<typename... Strings>
static constexpr auto make_simple_string_hash(Strings&&...) {
  return simple_string_hash<Strings...>{};
}
