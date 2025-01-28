#pragma once

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <stdexcept>

class Vector {
   public:
    class Iterator {
       public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = int;
        using difference_type = ptrdiff_t;
        using pointer = int*;
        using reference = int&;

        Iterator() = default;

        explicit Iterator(pointer iter) : iter_(iter) {
        }

        reference operator*() const {
            return *iter_;
        }

        pointer operator->() const {
            return iter_;
        }

        Iterator& operator+=(difference_type n) {
            iter_ += n;
            return *this;
        }

        Iterator& operator-=(difference_type n) {
            iter_ -= n;
            return *this;
        }

        Iterator operator+(difference_type n) const {
            return Iterator(iter_ + n);
        }

        friend Iterator operator+(difference_type n, const Iterator& it) {
            return Iterator(it.iter_ + n);
        }

        Iterator operator-(difference_type n) const {
            return Iterator(iter_ - n);
        }

        difference_type operator-(const Iterator& other) const {
            return iter_ - other.iter_;
        }

        reference operator[](difference_type n) const {
            return *(iter_ + n);
        }

        Iterator& operator++() {
            ++iter_;
            return *this;
        }

        Iterator operator++(int) {
            auto old_it = *this;
            ++iter_;
            return old_it;
        }

        Iterator& operator--() {
            --iter_;
            return *this;
        }

        Iterator operator--(int) {
            auto old_it = *this;
            --iter_;
            return old_it;
        }

        auto operator<=>(const Iterator& /*unused*/) const = default;

       private:
        pointer iter_ = nullptr;
    };

    Vector() = default;

    ~Vector() {
        delete[] data_;
    }

    explicit Vector(size_t n) : size_(n), capacity_(n) {
        data_ = new int[capacity_];  // NOLINT(cppcoreguidelines-owning-memory)
        for (size_t i = 0; i != size_; ++i) {
            auto* iter = data_;
            std::advance(iter, i);
            *iter = 0;
        }
    }

    Vector(std::initializer_list<int> input) : size_(input.size()), capacity_(input.size()) {
        data_ = new int[capacity_];  // NOLINT(cppcoreguidelines-owning-memory)
        for (size_t i = 0; i != size_; ++i) {
            int* iter = data_;
            const auto* iter_list = input.begin();
            std::advance(iter, i);
            std::advance(iter_list, i);
            *iter = *iter_list;
        }
    }

    Vector(const Vector& copy) : size_(copy.size_), capacity_(copy.capacity_) {
        if (size_ > 0) {
            data_ = new int[capacity_];  // NOLINT(cppcoreguidelines-owning-memory)
            for (size_t i = 0; i != copy.size_; ++i) {
                auto* iter_this = data_;
                const auto* iter_copy = copy.data_;
                std::advance(iter_copy, i);
                std::advance(iter_this, i);
                *iter_this = *iter_copy;
            }
        } else {
            data_ = nullptr;
        }
    }

    Vector(Vector&& copy) noexcept
        : data_(copy.data_), size_(copy.size_), capacity_(copy.capacity_) {
        copy.data_ = nullptr;
        copy.size_ = 0;
        copy.capacity_ = 0;
    }

    Vector& operator=(const Vector& other) {
        if (this == &other) {
            return *this;
        }
        delete[] data_;
        data_ = new int[other.capacity_];  // NOLINT(cppcoreguidelines-owning-memory)
        size_ = other.size_;
        capacity_ = other.capacity_;
        for (size_t i = 0; i != other.size_; ++i) {
            auto* iter_this = data_;
            const auto* iter_copy = other.data_;
            std::advance(iter_copy, i);
            std::advance(iter_this, i);
            *iter_this = *iter_copy;
        }
        return *this;
    }

    Vector& operator=(Vector&& other) noexcept {
        if (this != &other) {
            delete[] data_;

            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;

            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }

    int& operator[](const size_t& index) {
        int* iter = data_;
        std::advance(iter, index);
        return *iter;
    }

    const int& operator[](const size_t& index) const {
        const int* iter = data_;
        std::advance(iter, index);
        return *iter;
    }

    [[nodiscard]] Iterator begin() const {
        return Iterator(data_);
    }

    [[nodiscard]] Iterator end() const {
        auto* iter = data_;
        std::advance(iter, size_);
        return Iterator(iter);
    }

    void Swap(Vector& other) {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    [[nodiscard]] size_t Size() const {
        return size_;
    }

    [[nodiscard]] size_t Capacity() const {
        return capacity_;
    }

    void PushBack(const int& element) {
        if (size_ == capacity_) {
            size_t new_capacity = 0;
            if (capacity_ == 0) {
                new_capacity = 1;
            } else {
                new_capacity = capacity_ * 2;
            }
            int* data_new = new int[new_capacity];  // NOLINT(cppcoreguidelines-owning-memory)

            for (size_t i = 0; i != size_; ++i) {
                int* iter_this = data_;
                int* iter_new = data_new;
                std::advance(iter_this, i);
                std::advance(iter_new, i);
                *iter_new = *iter_this;
            }
            delete[] data_;
            data_ = data_new;
            capacity_ = new_capacity;
        }
        auto* iter = data_;
        std::advance(iter, size_);
        *iter = element;
        ++size_;
    }

    void PopBack() {
        if (size_ > 0) {
            --size_;
        } else {
            throw std::underflow_error("Cannot pop from empty Vector!");
        }
    }

    void Clear() {
        size_ = 0;
    }

    void Reserve(const size_t& new_capacity) {
        if (capacity_ < new_capacity) {
            auto* new_data =  // NOLINT(cppcoreguidelines-owning-memory)
                new int[new_capacity];
            for (size_t i = 0; i != size_; ++i) {
                auto* iter_this = data_;
                auto* iter_new = new_data;
                std::advance(iter_this, i);
                std::advance(iter_new, i);
                *iter_new = *iter_this;
            }
            delete[] data_;
            data_ = new_data;
            capacity_ = new_capacity;
        }
    }

   private:
    int* data_ = nullptr;
    size_t size_ = 0;
    size_t capacity_ = 0;
};
