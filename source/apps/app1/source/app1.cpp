#include "../include/app1.h"

#include <iostream>

#include "module1.h"

void app1() { std::cout << module1::hello() << std::endl; }

int app1_main() {
    app1();
    return 0;
}