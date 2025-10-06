#pragma once

#include "../SmartPtrs.h"
#include "../ArraySeq.h"

#include <optional>
#include <functional>
#include <limits>
#include <utility>

template <typename T, typename Generator, typename Mem>
class LazySeq {
private:
    mutable Mem mem_; 
    mutable Generator gen_; 
    size_t max_len_;

public:
    static constexpr size_t INF = std::numeric_limits<size_t>::max();

public:
    explicit LazySeq(Generator gen, Mem mem, size_t max_len = INF) 
        : mem_(std::move(mem)), gen_(std::move(gen)), max_len_(max_len) {}


    LazySeq(const LazySeq& other) 
        : mem_(other.mem_), gen_(other.gen_), max_len_(other.max_len_) {}


    LazySeq(LazySeq&& other) : mem_(std::move(other.mem_)), 
          gen_(std::move(other.gen_)), 
          max_len_(other.max_len_) {}


    LazySeq& operator=(const LazySeq& other) {
        if (this != &other) {
            mem_ = other.mem_;
            gen_ = other.gen_;
            max_len_ = other.max_len_;
        }
        return *this;
    }

    LazySeq& operator=(LazySeq&& other) noexcept {
        if (this != &other) {
            mem_ = std::move(other.mem_);
            gen_ = std::move(other.gen_);
            max_len_ = other.max_len_;
        }
        return *this;
    }

    std::optional<std::reference_wrapper<T>> Get(size_t i) {
        auto hit = mem_.Get(i);
        if (hit) {
            return hit;
        }
        while (mem_.MaterializedCount() <= i && mem_.MaterializedCount() < max_len_) {
            try {
                T next = gen_.GetNext();
                mem_.Append(std::move(next));
            } catch (...) {
                break;
            }
        }

        return mem_.Get(i);
    }

    std::optional<std::reference_wrapper<const T>> Get(size_t i) const {
        auto hit = mem_.Get(i);
        if (hit) {
            return hit;
        }
        while (mem_.MaterializedCount() <= i && mem_.MaterializedCount() < max_len_) {
            try {
                T next = gen_.GetNext();
                mem_.Append(std::move(next));
            } catch (...) {
                break;
            }
        }

        return mem_.Get(i);
    }

    size_t MaterializedCount() const noexcept { 
        return mem_.MaterializedCount(); 
    }
    
    size_t MaxLen() const noexcept { 
        return max_len_; 
    }
};
