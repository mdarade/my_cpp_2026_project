#include <iostream>
#include <tuple>

#include "cppdemo/print_all.hpp"

int main() {
  cppdemo::print_all(std::cout, std::make_tuple(1, 3.14, "Hello C++26"));
}
