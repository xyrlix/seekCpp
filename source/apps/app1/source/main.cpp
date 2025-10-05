#include <cstring>
#include <iostream>

#include "../include/app1.h"

int main() {
    app1_main();

    // dst < src
    int arr1[5] = {1, 2, 3, 4, 5};
    memcpy(arr1, arr1 + 1, 3 * sizeof(int));   // 目标地址在源地址之前且有重叠
    for (int i = 0; i < 5; i++) {
        std::cout << arr1[i] << " ";
    }
    std::cout << std::endl;

    // dst > src
    int arr2[5] = {1, 2, 3, 4, 5};
    memcpy(arr2 + 1, arr2, 3 * sizeof(int));   // 目标地址在源地址之后且有重叠
    for (int i = 0; i < 5; i++) {
        std::cout << arr2[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}