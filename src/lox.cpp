#include "lox.hpp"

#include "context.hpp"

#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>

namespace lox {

namespace {

void run([[maybe_unused]] Context& ctx, std::string code) {
  std::println("received code: {}", code);
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
