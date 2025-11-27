#include "lox.hpp"

#include "context.hpp"
#include "keywords.hpp"
#include "scanner.hpp"
#include "token.hpp"

#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

namespace lox {

namespace {

void run(Context& ctx, std::string code) {
  std::vector<Token> tokens = scanner::scan_tokens(ctx, code);

  if (ctx.had_error) {
    std::println("lox: encountered error while scanning");
    return;
  }

  std::println("Received tokens:");

  for (const Token& token : tokens) {
    std::string literal = std::invoke([&]() -> std::string {
      if (std::holds_alternative<std::string>(token.literal)) {
        return std::format(" (str literal: {})", std::get<std::string>(token.literal));
      } else if (std::holds_alternative<double>(token.literal)) {
        return std::format(" (num literal: {})", std::get<double>(token.literal));
      } else {
        return "";
      }
    });

    std::print("- {}{}", token.lexeme, literal);

    if (token.type == Token::Type::IDENTIFIER) {
      std::println(" (identifier)");
    } else if (is_keyword_enum(token.type)) {
      std::println(" (keyword)");
    } else {
      std::println();
    }
  }
}

}  // namespace

void run_file(Context& ctx, std::filesystem::path file_path) {
  if (!std::filesystem::exists(file_path)) {
    throw std::runtime_error(
        std::format("file \"{}\" does not exist", std::filesystem::absolute(file_path).string()));
  }

  std::ifstream file(file_path);

  if (!file.is_open()) {
    throw std::runtime_error("could not open file");
  }

  run(ctx, std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()));

  if (ctx.had_error) {
    return;
  }
}

void run_prompt(Context& ctx) {
  std::string line;

  while (true) {
    std::print(std::cout, "> ");

    if (!std::getline(std::cin, line)) {
      break;
    }

    run(ctx, line);

    // Reset the error flag on every loop iteration because it shouldn't carry over
    ctx.had_error = false;
  }
}

}  // namespace lox
