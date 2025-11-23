#include "scanner.hpp"

#include "log.hpp"
#include "token.hpp"

#include <format>
#include <optional>
#include <variant>
#include <vector>

namespace lox::scanner {

std::vector<Token> scan_tokens(Context& ctx, const std::string& source) {
  size_t start = 0;    // The first character in the lexeme being scanned
  size_t current = 0;  // The character currently being considered
  size_t line = 1;     // Incremented every time we meet a newline

  std::vector<Token> tokens;

  const auto is_at_end = [&]() -> bool { return current >= source.length(); };

  // Conditionally advances the current position if the current character matches what we expect
  const auto match = [&](char expected) -> bool {
    if (is_at_end() || source[current] != expected) {
      return false;
    }

    current++;
    return true;
  };

  // Performs one character of lookahead without any advancing
  const auto peek = [&]() -> char {
    if (is_at_end()) {
      return '\0';
    }

    return source[current];
  };

  const auto add_token = [&](Token::Type type, std::optional<Token::Literal> literal = {}) -> void {
    tokens.push_back({
        .type = type,
        .lexeme = source.substr(start, current - start),
        .literal = literal.value_or<Token::Literal>({}),
        .line = static_cast<int>(line),
    });
  };

  while (!is_at_end()) {
    // We're at the beginning of the next lexeme
    start = current;

    // Get the current character and then increment the position
    switch (source[current++]) {
      using enum Token::Type;

        // clang-format off
      case '(': add_token(LEFT_PAREN); break;
      case ')': add_token(RIGHT_PAREN); break;
      case '{': add_token(LEFT_BRACE); break;
      case '}': add_token(RIGHT_BRACE); break;
      case ',': add_token(COMMA); break;
      case '.': add_token(DOT); break;
      case '-': add_token(MINUS); break;
      case '+': add_token(PLUS); break;
      case ';': add_token(SEMICOLON); break;
      case '*': add_token(STAR); break;

      case '!': add_token(match('=') ? BANG_EQUAL : BANG); break;
      case '=': add_token(match('=') ? EQUAL_EQUAL : EQUAL); break;
      case '<': add_token(match('=') ? LESS_EQUAL : LESS); break;
      case '>': add_token(match('=') ? GREATER_EQUAL : GREATER); break;
        // clang-format on

      case '/':
        if (match('/')) {
          // Skips comments. Comments go until the end of the line
          while (peek() != '\n' && !is_at_end()) {
            current++;
          }
        } else {
          add_token(SLASH);
        }
        break;

      case ' ':
      case '\r':
      case '\t':
        // Skip newlines and whitespace
        break;

      case '\n':
        line++;
        break;

      default:
        // Note that we don't stop on an error, we keep scanning
        log::error(ctx, static_cast<int>(line),
                   std::format("unexpected character \"{}\".", source[current]));
        break;
    }
  }

  // Add a final end of file token
  tokens.push_back({
      .type = Token::Type::END_OF_FILE,
      .lexeme = "",
      .literal = {},
      .line = static_cast<int>(line),
  });

  return tokens;
}

}  // namespace lox::scanner
