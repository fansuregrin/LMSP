#include <thread>
#include <vector>
#include <cassert>
#include <iostream>
#include "counter.hpp"
#include "../common/utils.hpp"

void testGetAndIncrease() {
    Counter c;
    std::vector<long> arr1, arr2;

    std::thread t1([&] {
        for (int i=0; i<5; ++i) {
            arr1.push_back(c.getAndIncrease());
            std::this_thread::yield();
        }
    });
    std::thread t2([&] {
        for (int i=0; i<5; ++i) {
            arr2.push_back(c.getAndIncrease());
            std::this_thread::yield();
        }
    });
    t1.join();
    t2.join();

    std::cout << "arr1: " << arr1 << std::endl;
    std::cout << "arr2: " << arr2 << std::endl;

    assert(c.getValue() == 10);
}

int main() {
    testGetAndIncrease();
}