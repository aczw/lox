#pragma once

#include <cstdint>
#include <string>
#include <variant>

namespace lox {

struct Token {
  /// The literal values that a token can represent.
  using Literal = std::variant<std::monostate, std::string, double>;

  enum class Type : uint8_t {
    // clang-format off
    /// Single-character tokens.
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

    /// One or two character tokens.
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,

    /// Literals.
    IDENTIFIER, STRING, NUMBER,

    /// Keywords.
    AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
    PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

    /// End of file. Naming it `EOF` gives me (false) errors for some reason.
    END_OF_FILE,
    // clang-format on
  };

  Type type = Type::END_OF_FILE;
  std::string lexeme;
  Literal literal;
  int line = 0;
};

}  // namespace lox
