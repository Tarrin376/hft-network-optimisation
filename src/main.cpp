#include <iostream>
#include "test.h"

int main() {
    std::cout << "Testing" << '\n';
    std::cout << foo() << '\n';
    return 0;
}

int foo() {
    return 5;
}