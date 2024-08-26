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

template <typename F>
void testSwap(F swap) {
    Counter a, b;
    a.getAndIncrease();
    long orgA = a.getValue();
    long orgB = b.getValue();

    std::thread t1([&] {
        swap(a, b);
    });
    std::thread t2([&] {
        swap(b, a);
    });
    t1.join();
    t2.join();
    assert(a.getValue() == orgA && b.getValue() == orgB);
}

int main() {
    testGetAndIncrease();
    testSwap<decltype(swapV1)>(swapV1);
    testSwap<decltype(swapV2)>(swapV2);
    testSwap<decltype(swapV3)>(swapV3);
}