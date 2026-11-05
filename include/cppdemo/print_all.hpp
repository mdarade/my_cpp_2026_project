#pragma once

#include <ostream>

namespace cppdemo {

// Prints each element of a tuple on its own line. C++26 `template for` unrolls
// the loop at compile time, so each element keeps its own type.
void print_all(std::ostream& out, const auto& tuple) {
  template for (const auto& item : tuple) { out << item << '\n'; }
}

}  // namespace cppdemo
