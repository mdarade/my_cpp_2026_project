include(FetchContent)

# GoogleTest pinned to a release + checksum. SYSTEM keeps our warnings out of its headers.
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.tar.gz
  URL_HASH SHA256=65fab701d9829d38cb77c14acdc431d2108bfdbf8979e40eb8ae567edf10b27c
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  SYSTEM
)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(print_all_test print_all_test.cpp)
target_link_libraries(print_all_test PRIVATE cppdemo::cppdemo cppdemo_warnings GTest::gtest_main)
set_target_properties(print_all_test PROPERTIES CXX_CLANG_TIDY "${CPPDEMO_CLANG_TIDY}")

include(GoogleTest)
gtest_discover_tests(print_all_test)
