#pragma once

#include "../ArraySeq.h"

#include <optional>
#include <functional>

template <typename T>
class ArraySeqMem {
private:
    ArraySeq<T> cache_;

public:
    ArraySeqMem() = default;

    std::optional<std::reference_wrapper<T>> Get(size_t i) noexcept {
        if (i < cache_.size()) {
            return std::ref(cache_.at(i));
        }
        return std::nullopt;
    }

    std::optional<std::reference_wrapper<const T>> Get(size_t i) const noexcept {
        if (i < cache_.size()) {
            return std::cref(cache_.at(i));
        }
        return std::nullopt;
    }

    bool Has(size_t i) const noexcept { 
        return i < cache_.size(); 
    }

    void Append(const T& t) { 
        cache_.push_back(t); 
    }

    void Append(T&& t) { 
        cache_.push_back(std::move(t)); 
    }

    size_t MaterializedCount() const noexcept { 
        return cache_.size(); 
    }

    void Clear() { 
        cache_.clear(); 
    }
};
