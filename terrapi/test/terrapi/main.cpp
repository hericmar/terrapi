#include <iostream>

#include "fmt/format.h"

#include "tests.h"

int main()
{
    try {
        test_config();
        test_rules();
    } catch (std::exception& ex) {
        std::cout << ex.what() << std::endl;
    }
}
