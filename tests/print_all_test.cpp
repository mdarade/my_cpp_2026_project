#include "cppdemo/print_all.hpp"

#include <gtest/gtest.h>

#include <sstream>
#include <tuple>

TEST(PrintAllTest, PrintsEachElementOnItsOwnLine) {
  std::ostringstream out;
  cppdemo::print_all(out, std::make_tuple(1, 3.14, "Hello C++26"));
  EXPECT_EQ(out.str(), "1\n3.14\nHello C++26\n");
}
