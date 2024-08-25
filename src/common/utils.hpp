#include <iostream>
#include <vector>

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &vec) {
    const auto begin = vec.begin();
    for (auto it=begin; it != vec.end(); ++it) {
        if (it == begin) {
            os << *it;
        } else {
            os << ' ' << *it;
        }
    }
    return os;
}