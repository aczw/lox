#pragma once

#include "context.hpp"

#include <filesystem>

namespace lox {

void run_file(Context& ctx, std::filesystem::path file_path);

void run_prompt(Context& ctx);

}  // namespace lox
