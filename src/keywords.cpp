#include "keywords.hpp"

#include <algorithm>

namespace lox {

std::optional<Token::Type> is_keyword(std::string_view text) {
  if (const auto it = std::ranges::lower_bound(KEYWORDS, text, {}, &KeywordEntry::first);
      it != KEYWORDS.end() && it->first == text) {
    return it->second;
  }

  return std::nullopt;
}

bool is_keyword_enum(Token::Type type) {
  return std::ranges::contains(KEYWORDS, type, &KeywordEntry::second);
}

}  // namespace lox
