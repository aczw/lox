cmake_minimum_required(VERSION 4.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(lox VERSION 1.0.0 LANGUAGES CXX)

add_executable(lox src/main.cpp)

target_compile_features(lox PRIVATE cxx_std_23)
target_compile_options(lox PRIVATE -Wall -Wextra -Werror -Wpedantic -Wconversion)
set_target_properties(lox PROPERTIES cxx_extensions OFF cxx_standard_required ON)
