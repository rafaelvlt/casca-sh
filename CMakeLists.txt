cmake_minimum_required(VERSION 3.15...4.1)

project(casca LANGUAGES C)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*.c)

set(CMAKE_C_STANDARD 23)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(casca ${SOURCE_FILES})

find_library(READLINE_LIB readline)

if (NOT READLINE_LIB)
  message(FATAL_ERROR "readline lib not found! install the package libreadline-dev.")
endif()

target_link_libraries(casca PRIVATE ${READLINE_LIB})

target_include_directories(casca PRIVATE include)

target_compile_options(casca PRIVATE -Wall -Wextra -Wpedantic -Werror -fsanitize=address -g)

target_link_options(casca PRIVATE -fsanitize=address -static-libasan)
