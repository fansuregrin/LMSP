#include <mutex>

// 线程安全的 Counter
class Counter {
public:
    friend void swapV1(Counter &a, Counter &b);

    Counter() : _value(0) {}

    long getValue() const {
        std::lock_guard<std::mutex> lck(_mtx);
        return _value;
    }

    long getAndIncrease() {
        std::lock_guard<std::mutex> lck(_mtx);
        long ret = _value++;
        return ret;
    }
private:
    mutable std::mutex _mtx;
    long _value;
};

void swapV1(Counter &a, Counter &b) {
    // 可能会死锁
    // 线程 1：swapV1(a, b);
    // 线程 2：swpaV1(b, a);
    std::lock_guard<std::mutex> lckA(a._mtx);
    std::lock_guard<std::mutex> lckB(a._mtx);
    long v = a._value;
    a._value = b._value;
    b._value = v;
}