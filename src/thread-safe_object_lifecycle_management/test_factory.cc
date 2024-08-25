#include "factory.hpp"
#include <cassert>

template <typename F>
void testFactoryGet(const std::string &className) {
    std::cout << "\n===== Test Factory get [" << className << "] =====\n";
    std::shared_ptr<F> factory = std::make_shared<F>();
    {
        std::shared_ptr<Stock> stk1 = factory->get("stock1");
        std::shared_ptr<Stock> stk2 = factory->get("stock2");
    }
}

template <typename F>
void testLongLifeFactory(const std::string &className) {
    std::cout << "\n===== Test Long Life Factory [" << className << "] =====\n";
    std::shared_ptr<F> factory = std::make_shared<F>();
    {
        std::shared_ptr<Stock> stk1 = factory->get("NYSE:IBM");
        std::shared_ptr<Stock> stk2 = factory->get("NYSE:IBM");
        assert(stk1 == stk2);
        // Stock 对象在这里销毁
    }
    // StockFactory 对象在这里销毁
}

template <typename F>
void testShortLifeFactory(const std::string &className) {
    std::cout << "\n===== Test Short Life Factory [" << className << "] =====\n";
    std::shared_ptr<Stock> stk1;
    {
        std::shared_ptr<F> factory =
            std::make_shared<F>();
        stk1 = factory->get("NYSE:IBM");
        std::shared_ptr<Stock> stk2 = factory->get("NYSE:IBM");
        assert(stk1 == stk2);
        // StockFactory 对象在这里销毁
    }
    // Stock 对象在这里销毁
}

template <typename F>
void testFactoryGetSize(const std::string &className) {
    std::cout << "\n===== Test Factory getSize [" << className << "] =====\n";
    std::shared_ptr<F> factory = std::make_shared<F>();
    factory->get("stock1");
    factory->get("stock2");
    factory->get("stock3");
    std::cout << "current factory size: " << factory->getSize() << std::endl;
}

int main() {
    testFactoryGet<version1::StockFactory>("version1::StockFactory");
    testFactoryGet<version2::StockFactory>("version2::StockFactory");
    testFactoryGet<version3::StockFactory>("version3::StockFactory");
    testFactoryGet<version4::StockFactory>("version4::StockFactory");
    testFactoryGet<StockFactory>("StockFactory");

    testFactoryGetSize<version1::StockFactory>("version1::StockFactory");
    testFactoryGetSize<version2::StockFactory>("version2::StockFactory");
    testFactoryGetSize<version3::StockFactory>("version3::StockFactory");
    testFactoryGetSize<version4::StockFactory>("version4::StockFactory");
    testFactoryGetSize<StockFactory>("StockFactory");

    testLongLifeFactory<version1::StockFactory>("version1::StockFactory");
    testLongLifeFactory<version2::StockFactory>("version2::StockFactory");
    testLongLifeFactory<version3::StockFactory>("version3::StockFactory");
    testLongLifeFactory<version4::StockFactory>("version4::StockFactory");
    testLongLifeFactory<StockFactory>("StockFactory");

    testShortLifeFactory<version1::StockFactory>("version1::StockFactory");
    testShortLifeFactory<version2::StockFactory>("version2::StockFactory");
    testShortLifeFactory<version3::StockFactory>("version3::StockFactory");
    testShortLifeFactory<version4::StockFactory>("version4::StockFactory");
    testShortLifeFactory<StockFactory>("StockFactory");
}