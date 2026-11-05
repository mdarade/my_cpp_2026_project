cmake_minimum_required(VERSION 3.28)
project(CppDemo VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# Writes compile_commands.json so clangd/clang-tidy see the exact compile flags.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(CPPDEMO_WARNINGS_AS_ERRORS "Treat compiler and clang-tidy warnings as errors" OFF)
option(CPPDEMO_ENABLE_SANITIZERS "Build with AddressSanitizer + UndefinedBehaviorSanitizer" OFF)
option(CPPDEMO_ENABLE_CLANG_TIDY "Run clang-tidy on project sources while compiling" OFF)
option(CPPDEMO_BUILD_TESTS "Build unit tests" ON)
option(CPPDEMO_USE_LIBCXX "Use LLVM libc++ (most complete C++26 library support with clang)" ON)

# --- Standard library: applied globally so GoogleTest uses the same one (ABI must match).
if(CPPDEMO_USE_LIBCXX AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  add_compile_options(-stdlib=libc++)
  add_link_options(-stdlib=libc++)
endif()

# --- Sanitizers: applied globally so third-party code (GoogleTest) is instrumented too.
if(CPPDEMO_ENABLE_SANITIZERS)
  add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer -fno-sanitize-recover=all)
  add_link_options(-fsanitize=address,undefined)
endif()

# --- Warnings: applied only to our own targets via this interface library.
add_library(cppdemo_warnings INTERFACE)
target_compile_options(cppdemo_warnings INTERFACE
  -Wall -Wextra -Wpedantic
  -Wshadow -Wconversion -Wsign-conversion
  -Wold-style-cast -Wnon-virtual-dtor -Woverloaded-virtual
  -Wimplicit-fallthrough -Wnull-dereference
  $<$<BOOL:${CPPDEMO_WARNINGS_AS_ERRORS}>:-Werror>
)

# --- clang-tidy: attached per-target (CXX_CLANG_TIDY) so it skips third-party code.
set(CPPDEMO_CLANG_TIDY "")
if(CPPDEMO_ENABLE_CLANG_TIDY)
  find_program(CLANG_TIDY_EXE NAMES clang-tidy REQUIRED)
  set(CPPDEMO_CLANG_TIDY "${CLANG_TIDY_EXE}")
  if(CPPDEMO_WARNINGS_AS_ERRORS)
    list(APPEND CPPDEMO_CLANG_TIDY "--warnings-as-errors=*")
  endif()
endif()

# --- `cmake --build --preset debug --target format` formats all sources in place.
find_program(CLANG_FORMAT_EXE NAMES clang-format)
if(CLANG_FORMAT_EXE)
  file(GLOB_RECURSE CPPDEMO_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp)
  add_custom_target(format
    COMMAND ${CLANG_FORMAT_EXE} -i ${CPPDEMO_SOURCES}
    COMMENT "Formatting sources with clang-format"
    VERBATIM)
endif()

# --- Library (header-only)
add_library(cppdemo INTERFACE)
add_library(cppdemo::cppdemo ALIAS cppdemo)
target_include_directories(cppdemo INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# --- Application
add_executable(app src/main.cpp)
target_link_libraries(app PRIVATE cppdemo::cppdemo cppdemo_warnings)
set_target_properties(app PROPERTIES CXX_CLANG_TIDY "${CPPDEMO_CLANG_TIDY}")

# --- Tests
if(CPPDEMO_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
