#include "token.hpp"

#include <array>
#include <optional>
#include <string_view>
#include <utility>

namespace lox {

using KeywordEntry = std::pair<std::string_view, Token::Type>;

/// Maps keywords to their corresponding enum value. Entries have been sorted alphabetically by
/// the reserved word, enabling us to perform binary search on the array.
constexpr std::array KEYWORDS = std::to_array<KeywordEntry>({
    {"and", Token::Type::AND},
    {"class", Token::Type::CLASS},
    {"else", Token::Type::ELSE},
    {"false", Token::Type::FALSE},
    {"for", Token::Type::FOR},
    {"fun", Token::Type::FUN},
    {"if", Token::Type::IF},
    {"nil", Token::Type::NIL},
    {"or", Token::Type::OR},
    {"print", Token::Type::PRINT},
    {"return", Token::Type::RETURN},
    {"super", Token::Type::SUPER},
    {"this", Token::Type::THIS},
    {"true", Token::Type::TRUE},
    {"var", Token::Type::VAR},
    {"while", Token::Type::WHILE},
});

/// Checks if a string is a valid reserved keyword via binary search, and returns the corresponding
/// enum value if so.
std::optional<Token::Type> is_keyword(std::string_view text);

/// Checks if a token enum is for a reserved keyword.
bool is_keyword_enum(Token::Type type);

}  // namespace lox
