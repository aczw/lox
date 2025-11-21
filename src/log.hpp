#pragma once

#include "context.hpp"

#include <string_view>

namespace lox::log {

void error(Context& ctx, int line_number, std::string_view message);

void report(Context& ctx, int line_number, std::string_view where, std::string_view message);

}  // namespace lox::log
