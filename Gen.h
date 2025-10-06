#pragma once
#include <utility>

template <typename T, typename Func>
class GenFn {
private:
    Func next_;

public:
    explicit GenFn(Func f) : next_(std::move(f)) {}
    
    T GetNext() { return next_(); }
};
