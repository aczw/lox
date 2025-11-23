#pragma once

#include "context.hpp"
#include "token.hpp"

#include <string>
#include <vector>

namespace lox::scanner {

std::vector<Token> scan_tokens(Context& ctx, const std::string& source);

}
