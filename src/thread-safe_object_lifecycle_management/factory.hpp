#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

using namespace std::placeholders;

class Stock {
public:
    Stock(const std::string &name) : _name(name) {
        std::cout << "Stock() " << this << " " << _name << std::endl;
    }

    Stock(const Stock &) = delete;

    Stock &operator=(const Stock &) = delete;

    ~Stock() { std::cout << "~Stock() " << this << " " << _name << std::endl; }

    const std::string &key() { return _name; }

private:
    std::string _name;
};

namespace version1 {

class StockFactory {
public:
    StockFactory() = default;

    StockFactory(const StockFactory &) = delete;

    StockFactory &operator=(const StockFactory &) = delete;

    ~StockFactory() { std::cout << "call ~StockFactory()\n"; }

    std::shared_ptr<Stock> get(const std::string &key) {
        std::lock_guard<std::mutex> lck(_mtx);
        // 如果在 _stocks 中找到 key，就返回 _stocks[key]；
        // 否则，就新建一个 Stock
        std::shared_ptr<Stock> &pStock = _stocks[key];
        if (!pStock) {
            pStock.reset(new Stock(key));
        }
        return pStock;
    }

    size_t getSize() {
        std::lock_guard<std::mutex> lck(_mtx);
        return _stocks.size();
    }

private:
    mutable std::mutex _mtx;

    // _stocks 中保存的是 shared_ptr<Stock>，Stock 对象不会被销毁
    std::unordered_map<std::string, std::shared_ptr<Stock>> _stocks;
};

} // namespace version1

namespace version2 {

class StockFactory {
public:
    StockFactory() = default;

    StockFactory(const StockFactory &) = delete;

    StockFactory &operator=(const StockFactory &) = delete;

    ~StockFactory() { std::cout << "call ~StockFactory()\n"; }

    std::shared_ptr<Stock> get(const std::string &key) {
        std::shared_ptr<Stock> pStock;
        std::lock_guard<std::mutex> lck(_mtx);
        std::weak_ptr<Stock> &wkStock = _stocks[key];
        pStock = wkStock.lock();
        if (!pStock) {
            pStock.reset(new Stock(key));
            wkStock = pStock;
        }
        return pStock;
    }

    size_t getSize() {
        std::lock_guard<std::mutex> lck(_mtx);
        return _stocks.size();
    }

private:
    mutable std::mutex _mtx;

    std::unordered_map<std::string, std::weak_ptr<Stock>> _stocks;
};

} // namespace version2

namespace version3 {

class StockFactory {
public:
    StockFactory() = default;

    StockFactory(const StockFactory &) = delete;

    StockFactory &operator=(const StockFactory &) = delete;

    ~StockFactory() { std::cout << "call ~StockFactory()\n"; }

    std::shared_ptr<Stock> get(const std::string &key) {
        std::shared_ptr<Stock> pStock;
        std::lock_guard<std::mutex> lck(_mtx);
        std::weak_ptr<Stock> &wkStock = _stocks[key];
        pStock = wkStock.lock();
        if (!pStock) {
            // 希望这个 StockFactory 对象比所有的 Stock 对象都活得长
            // 否则，调用 deleteStock 会出现未定义行为
            pStock.reset(new Stock(key),
                         std::bind(&StockFactory::deleteStock, this, _1));
            wkStock = pStock;
        }
        return pStock;
    }

    size_t getSize() {
        std::lock_guard<std::mutex> lck(_mtx);
        return _stocks.size();
    }

private:
    void deleteStock(Stock *stock) {
        std::cout << "call deteleStock(): delete Stock<" << stock << ">\n";
        if (stock) {
            std::lock_guard<std::mutex> lck(_mtx);
            _stocks.erase(stock->key());
        }
        delete stock;
    }

private:
    mutable std::mutex _mtx;

    std::unordered_map<std::string, std::weak_ptr<Stock>> _stocks;
};

} // namespace version3

namespace version4 {

class StockFactory : public std::enable_shared_from_this<StockFactory> {
public:
    StockFactory() = default;

    StockFactory(const StockFactory &) = delete;

    StockFactory &operator=(const StockFactory &) = delete;

    ~StockFactory() { std::cout << "call ~StockFactory()\n"; }

    std::shared_ptr<Stock> get(const std::string &key) {
        std::shared_ptr<Stock> pStock;
        std::lock_guard<std::mutex> lck(_mtx);
        std::weak_ptr<Stock> &wkStock = _stocks[key];
        pStock = wkStock.lock();
        if (!pStock) {
            // 特意延长了 StockFactory 的生命周期
            pStock.reset(new Stock(key), std::bind(&StockFactory::deleteStock,
                                                   shared_from_this(), _1));
            wkStock = pStock;
        }
        return pStock;
    }

    size_t getSize() {
        std::lock_guard<std::mutex> lck(_mtx);
        return _stocks.size();
    }

private:
    void deleteStock(Stock *stock) {
        std::cout << "call deteleStock(): delete Stock<" << stock << ">\n";
        if (stock) {
            std::lock_guard<std::mutex> lck(_mtx);
            _stocks.erase(stock->key()); // 这是错误的实现
        }
        delete stock;
    }

private:
    mutable std::mutex _mtx;

    std::unordered_map<std::string, std::weak_ptr<Stock>> _stocks;
};

} // namespace version4


class StockFactory : public std::enable_shared_from_this<StockFactory> {
public:
    StockFactory() = default;

    StockFactory(const StockFactory &) = delete;

    StockFactory &operator=(const StockFactory &) = delete;

    ~StockFactory() { std::cout << "call ~StockFactory()\n"; }

    std::shared_ptr<Stock> get(const std::string &key) {
        std::shared_ptr<Stock> pStock;
        std::lock_guard<std::mutex> lck(_mtx);
        std::weak_ptr<Stock> &wkStock = _stocks[key];
        pStock = wkStock.lock();
        if (!pStock) {
            // 将 shared_ptr 强制转型为 weak_ptr
            pStock.reset(new Stock(key), std::bind(&StockFactory::weakDeleteCallback,
                std::weak_ptr<StockFactory>(shared_from_this()), _1));
            wkStock = pStock;
        }
        return pStock;
    }

    size_t getSize() {
        std::lock_guard<std::mutex> lck(_mtx);
        return _stocks.size();
    }

private:
    static void weakDeleteCallback(const std::weak_ptr<StockFactory> &wkFactory,
                                   Stock *stock) {
        
        std::cout << "call weakDeleteCallback(): delete Stock<" << stock << ">\n";
        std::shared_ptr<StockFactory> factory(wkFactory.lock());
        if (factory) {
            factory->removeStock(stock);
        } else {
            std::cout << "StockFactory died!\n";
        }
        delete stock;
    }

    void removeStock(Stock *stock) {
        if (stock) {
            std::lock_guard<std::mutex> lck(_mtx);
            auto it = _stocks.find(stock->key());
            if (it != _stocks.end() && it->second.expired()) {
                _stocks.erase(it);
            }
        }
    }

private:
    mutable std::mutex _mtx;

    std::unordered_map<std::string, std::weak_ptr<Stock>> _stocks;
};