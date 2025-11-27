#include "scanner.hpp"

#include "keywords.hpp"
#include "log.hpp"
#include "token.hpp"

#include <charconv>
#include <format>
#include <functional>
#include <optional>
#include <string_view>
#include <variant>
#include <vector>

namespace lox::scanner {

std::vector<Token> scan_tokens(Context& ctx, const std::string& source) {
  size_t start = 0;    // The first character in the lexeme being scanned
  size_t current = 0;  // The character currently being considered
  size_t line = 1;     // Incremented every time we meet a newline

  std::vector<Token> tokens;

  static const auto is_at_end = [&]() -> bool { return current >= source.length(); };

  // Conditionally advances the current position if the current character matches what we expect
  static const auto match = [&](char expected) -> bool {
    if (is_at_end() || source[current] != expected) {
      return false;
    }

    current++;
    return true;
  };

  // Performs one character of lookahead without any advancing
  static const auto peek = [&]() -> char {
    if (is_at_end()) {
      return '\0';
    }

    return source[current];
  };

  // Performs two characters of lookahead without any advancing
  static const auto peek_next = [&]() -> char {
    if (current + 1 >= source.length()) {
      return '\0';
    }

    return source[current + 1];
  };

  static const auto add_token = [&](Token::Type type,
                                    std::optional<Token::Literal> literal = {}) -> void {
    tokens.push_back({
        .type = type,
        .lexeme = source.substr(start, current - start),
        .literal = literal.value_or<Token::Literal>({}),
        .line = static_cast<int>(line),
    });
  };

  static const auto is_digit = [](char c) -> bool { return c >= '0' && c <= '9'; };

  static const auto is_alpha = [](char c) -> bool {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
  };

  static const auto is_alpha_numeric = [](char c) -> bool { return is_alpha(c) || is_digit(c); };

  while (!is_at_end()) {
    // We're at the beginning of the next lexeme
    start = current;

    // Get the current character and then increment the position
    char c = source[current++];

    switch (c) {
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

      case '/': {
        if (match('/')) {
          // Skip single-line comment. Comments go until the end of the line
          while (peek() != '\n' && !is_at_end()) {
            current++;
          }
        } else if (match('*')) {
          // Skip block comments, and support nested ones by keeping track of
          // which layer we're currently on. The initial block comment starts at layer 1
          int layer_count = 1;

          while (!is_at_end()) {
            char next = peek();
            char next_next = peek_next();

            if (next == '\n') {
              line++;
            } else if (next == '/' && next_next == '*') {
              // If we encounter another block comment start, increment the nest layer we're on
              layer_count++;
              current++;
            } else if (next == '*' && next_next == '/') {
              // If we encounter a block comment end, decrease the layer we're on
              layer_count--;

              // We've only finished parsing the initial block comment if we're currently not
              // within another nested block comment
              if (layer_count == 0) {
                current += 2;
                break;
              } else {
                // Otherwise, continue the while loop. Increment `current` once here and let the
                // other increment below take care of the second
                current++;
              }
            }

            current++;
          }
        } else {
          add_token(SLASH);
        }

        break;
      }

      case ' ':
      case '\r':
      case '\t':
        // Skip newlines and whitespace
        break;

      case '\n':
        line++;
        break;

      case '"': {
        // String literals
        while (peek() != '"' && !is_at_end()) {
          if (peek() == '\n') {
            line++;
          }

          current++;
        }

        if (is_at_end()) {
          log::error(ctx, static_cast<int>(line), "unterminated string.");
          break;
        }

        // Advance one more time to process the closing '"'
        current++;

        // For the value itself, we trim away the surrounding quotes
        add_token(Token::Type::STRING, source.substr(start + 1, current - start - 2));

        break;
      }

      default: {
        if (is_digit(c)) {
          // Process number literal
          while (is_digit(peek())) {
            current++;
          }

          // Look for a fractional part
          if (peek() == '.' && is_digit(peek_next())) {
            // Consume the '.'
            current++;

            while (is_digit(peek())) {
              current++;
            }
          }

          std::string number_substr = source.substr(start, current - start + 1);
          double value = 0.0;
          std::from_chars(number_substr.data(), number_substr.data() + number_substr.size(), value);

          add_token(Token::Type::NUMBER, value);
        } else if (is_alpha(c)) {
          // Begin by assuming any lexeme starting with a letter or underscore is an identifier...
          while (is_alpha_numeric(peek())) {
            current++;
          }

          // ...then check if the lexeme matches one of the reserved words. If so, update the token
          // type to be that of the specific reserved word instead.
          Token::Type type = std::invoke([&]() -> Token::Type {
            std::string lexeme = source.substr(start, current - start);

            if (auto type_opt = is_keyword(lexeme); type_opt) {
              return type_opt.value();
            }

            return Token::Type::IDENTIFIER;
          });

          add_token(type);
        } else {
          // Note that we don't stop on an error, we keep scanning
          log::error(ctx, static_cast<int>(line),
                     std::format("unexpected character \"{}\".", source[current]));
        }

        break;
      }
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
