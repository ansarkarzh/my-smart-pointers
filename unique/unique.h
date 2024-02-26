#pragma once

#include "compressed_pair.h"

#include <cstddef>  // std::nullptr_t
#include <utility>
#include <memory>

template <class T>
struct DefaultDelete {
    constexpr DefaultDelete() noexcept = default;

    template <typename U, typename = std::_Require<std::is_convertible<U*, T*>>>
    DefaultDelete(const DefaultDelete<U>&) noexcept {
    }

    void operator()(T* ptr) const {
        delete ptr;
    }
};

// Primary template
template <typename T, typename Deleter = DefaultDelete<T>>
class UniquePtr {
public:
    using LRef = typename std::add_lvalue_reference_t<T>;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    UniquePtr() noexcept : UniquePtr{nullptr} {
    }

    UniquePtr(T* ptr) noexcept : pair_{ptr} {};

    UniquePtr(T* ptr, const Deleter& deleter) noexcept : pair_{ptr, deleter} {};

    UniquePtr(T* ptr, Deleter&& deleter) noexcept : pair_{ptr, std::move(deleter)} {};

    UniquePtr(UniquePtr&& other) noexcept
        : pair_{other.Release(), std::move(other.pair_.GetSecond())} {};

    template <typename U, typename DeleterU>
    UniquePtr(UniquePtr<U, DeleterU>&& other) noexcept
        : pair_{other.Release(), std::move(other.GetDeleter())} {
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {
            Reset(other.Release());
            pair_.GetSecond() = std::move(other.pair_.GetSecond());
        }
        return *this;
    };

    UniquePtr& operator=(std::nullptr_t) noexcept {
        Reset();
        return *this;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~UniquePtr() noexcept {
        if (pair_.GetFirst()) {
            pair_.GetSecond()(pair_.GetFirst());
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    T* Release() noexcept {
        return std::exchange(pair_.GetFirst(), nullptr);
    };

    void Reset(T* ptr = nullptr) noexcept {
        T* old_ptr = std::exchange(pair_.GetFirst(), ptr);
        if (old_ptr) {
            pair_.GetSecond()(old_ptr);
        }
    };

    void Swap(UniquePtr& other) noexcept {
        T* temp_ptr = pair_.GetFirst();
        Deleter temp_del = std::move(pair_.GetSecond());
        pair_.GetFirst() = other.pair_.GetFirst();
        pair_.GetSecond() = std::move(other.pair_.GetSecond());
        other.pair_.GetFirst() = temp_ptr;
        other.pair_.GetSecond() = std::move(temp_del);
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const noexcept {
        return pair_.GetFirst();
    };

    Deleter& GetDeleter() noexcept {
        return pair_.GetSecond();
    };

    const Deleter& GetDeleter() const noexcept {
        return pair_.GetSecond();
    };

    explicit operator bool() const noexcept {
        return static_cast<bool>(pair_.GetFirst());
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Single-object dereference operators

    LRef operator*() const noexcept {
        return *pair_.GetFirst();
    };

    T* operator->() const noexcept {
        return pair_.GetFirst();
    };

private:
    CompressedPair<T*, Deleter> pair_;
};

// Specialization for arrays

template <class T>
struct DefaultDelete<T[]> {
    constexpr DefaultDelete() noexcept = default;

    template <typename U, typename = std::_Require<std::is_convertible<U (*)[], T (*)[]>>>
    DefaultDelete(const DefaultDelete<U[]>&) noexcept {
    }

    void operator()(T* ptr) const {
        delete[] ptr;
    }
};

template <typename T, typename Deleter>
class UniquePtr<T[], Deleter> : public UniquePtr<T, Deleter> {
public:
public:
    using LRef = typename std::add_lvalue_reference_t<T>;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    UniquePtr() noexcept : UniquePtr{nullptr} {
    }

    UniquePtr(T* ptr) noexcept : pair_{ptr} {};

    UniquePtr(T* ptr, const Deleter& deleter) noexcept : pair_{ptr, deleter} {};

    UniquePtr(T* ptr, Deleter&& deleter) noexcept : pair_{ptr, std::move(deleter)} {};

    UniquePtr(UniquePtr&& other) noexcept
        : pair_{other.Release(), std::move(other.pair_.GetSecond())} {};

    template <typename U, typename DeleterU>
    UniquePtr(UniquePtr<U, DeleterU>&& other) noexcept
        : pair_{other.Release(), std::move(other.GetDeleter())} {
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {
            Reset(other.Release());
            pair_.GetSecond() = std::move(other.pair_.GetSecond());
        }
        return *this;
    };

    UniquePtr& operator=(std::nullptr_t) noexcept {
        Reset();
        return *this;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~UniquePtr() noexcept {
        if (pair_.GetFirst()) {
            pair_.GetSecond()(pair_.GetFirst());
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    T* Release() noexcept {
        return std::exchange(pair_.GetFirst(), nullptr);
    };

    void Reset(T* ptr = nullptr) noexcept {
        T* old_ptr = std::exchange(pair_.GetFirst(), ptr);
        if (old_ptr) {
            pair_.GetSecond()(old_ptr);
        }
    };

    void Swap(UniquePtr& other) noexcept {
        T* temp_ptr = pair_.GetFirst();
        Deleter temp_del = std::move(pair_.GetSecond());
        pair_.GetFirst() = other.pair_.GetFirst();
        pair_.GetSecond() = std::move(other.pair_.GetSecond());
        other.pair_.GetFirst() = temp_ptr;
        other.pair_.GetSecond() = std::move(temp_del);
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const noexcept {
        return pair_.GetFirst();
    };

    Deleter& GetDeleter() noexcept {
        return pair_.GetSecond();
    };

    const Deleter& GetDeleter() const noexcept {
        return pair_.GetSecond();
    };

    explicit operator bool() const noexcept {
        return static_cast<bool>(pair_.GetFirst());
    };

    T& operator[](size_t ind) const noexcept {
        return UniquePtr::Get()[ind];
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Single-object dereference operators

    LRef operator*() const noexcept {
        return *pair_.GetFirst();
    };

    T* operator->() const noexcept {
        return pair_.GetFirst();
    };

private:
    CompressedPair<T*, Deleter> pair_;
};
