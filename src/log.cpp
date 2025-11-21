#include "log.hpp"

#include <print>

namespace lox::log {

void error(Context& ctx, int line_number, std::string_view message) {
  report(ctx, line_number, "", message);
}

void report(Context& ctx, int line_number, std::string_view where, std::string_view message) {
  std::println("[line {}] Error{}: {}", line_number, where, message);
  ctx.had_error = true;
}

}  // namespace lox::log
