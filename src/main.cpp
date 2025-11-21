#include "context.hpp"
#include "lox.hpp"

#include <cstdlib>
#include <exception>
#include <print>

int main(int argc, char* argv[]) {
  if (argc > 2) {
    std::println("usage: lox <script path>");
    return EXIT_FAILURE;
  }

  lox::Context ctx;

  try {
    if (argc == 2) {
      lox::run_file(ctx, argv[1]);
    } else {
      // Has to have no arguments
      lox::run_prompt(ctx);
    }
  } catch (const std::exception& ex) {
    std::println("lox: exception occurred: {}", ex.what());
    return EXIT_FAILURE;
  }

  return ctx.had_error ? EXIT_FAILURE : EXIT_SUCCESS;
}
