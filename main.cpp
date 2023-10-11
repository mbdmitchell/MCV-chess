#include <iostream>

#include "Location.h"

int main() {
    Location l;
    std::cout << Location<>::isHorizontal(Location{4,3}, Location{3,5});
    return 0;
}
