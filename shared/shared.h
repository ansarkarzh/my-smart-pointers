#pragma once

#include "sw_fwd.h"  // Forward declaration

#include <cstddef>  // std::nullptr_t
#include <memory>
#include <utility>
#include <cassert>

struct ControlBlockBase {
    void IncrementCounter() {
        ++counter_;
    };

    void DecrementCounter() {
        --counter_;
        if (counter_ == 0) {
            delete this;
        }
    };

    size_t GetCounter() const {
        return counter_;
    }

    virtual ~ControlBlockBase() = default;

private:
    size_t counter_ = 1;
};

template <typename T>
struct ControlBlockOwning : public ControlBlockBase {
    template <typename... Args>
    ControlBlockOwning(Args&&... args) : obj_{std::forward<Args>(args)...} {
    }

    T* GetPointer() {
        return &obj_;
    }

private:
    T obj_;
};

template <typename T>
struct ControlBlockPointer : public ControlBlockBase {
    ControlBlockPointer(T* ptr) : ptr_(ptr) {
    }

    ~ControlBlockPointer() override {
        delete ptr_;
    }

private:
    T* ptr_ = nullptr;
};

// https://en.cppreference.com/w/cpp/memory/shared_ptr
template <typename T>
class SharedPtr {
public:
    template <typename Y>
    friend class SharedPtr;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    SharedPtr() : observable_obj_{nullptr}, cb_{nullptr} {};

    SharedPtr(std::nullptr_t) : SharedPtr(){};

    SharedPtr(ControlBlockOwning<T>* ptr) : observable_obj_(ptr->GetPointer()), cb_(ptr){};

    explicit SharedPtr(T* ptr) {
        observable_obj_ = ptr;
        cb_ = new ControlBlockPointer(ptr);
    };

    template <typename U>
    explicit SharedPtr(U* ptr) {
        observable_obj_ = ptr;
        cb_ = new ControlBlockPointer<U>(ptr);
    }

    SharedPtr(const SharedPtr& other) {
        observable_obj_ = other.observable_obj_;
        cb_ = other.cb_;
        if (cb_) {
            cb_->IncrementCounter();
        }
    };

    SharedPtr(SharedPtr&& other)
        : observable_obj_(std::move(other.observable_obj_)), cb_(std::move(other.cb_)) {
        other.observable_obj_ = nullptr;
        other.cb_ = nullptr;
    };

    template <typename U>
    SharedPtr(SharedPtr<U>&& other) noexcept
        : observable_obj_(std::move(other.observable_obj_)), cb_(std::move(other.cb_)) {
        other.observable_obj_ = nullptr;
        other.cb_ = nullptr;
    }

    template <typename U>
    SharedPtr(SharedPtr<U>& other) noexcept {
        observable_obj_ = other.observable_obj_;
        cb_ = other.cb_;
        if (cb_) {
            cb_->IncrementCounter();
        }
    }

    // Aliasing constructor
    // #8 from https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr
    template <typename Y>
    SharedPtr(const SharedPtr<Y>& other, T* ptr) : observable_obj_(ptr), cb_(other.cb_) {
        if (cb_) {
            cb_->IncrementCounter();
        }
    }

    // Promote `WeakPtr`
    // #11 from https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr
    explicit SharedPtr(const WeakPtr<T>& other)
        : observable_obj_(other.observable_obj_), cb_(other.cb_) {
        if (!other.expired()) {
            cb_->IncrementCounter();
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    SharedPtr& operator=(const SharedPtr& other) {
        SharedPtr{other}.Swap(*this);
        return *this;
    };

    SharedPtr& operator=(SharedPtr& other) {
        SharedPtr{other}.Swap(*this);
        return *this;
    };

    SharedPtr& operator=(SharedPtr&& other) {
        SharedPtr{std::move(other)}.Swap(*this);
        return *this;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~SharedPtr() {
        if (cb_) {
            cb_->DecrementCounter();
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    void Reset() {
        SharedPtr{}.Swap(*this);
    };

    void Reset(T* ptr) {
        SharedPtr{ptr}.Swap(*this);
    };

    template <typename U>
    void Reset(U* ptr) {
        SharedPtr{ptr}.Swap(*this);
    }

    void Swap(SharedPtr& other) {
        std::swap(observable_obj_, other.observable_obj_);
        std::swap(cb_, other.cb_);
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const {
        return observable_obj_;
    };

    T& operator*() const {
        assert(observable_obj_ != nullptr);
        return *observable_obj_;
    };

    T* operator->() const {
        return observable_obj_;
    };

    size_t UseCount() const {
        if (cb_) {
            return cb_->GetCounter();
        }
        return 0;
    };

    explicit operator bool() const {
        if (observable_obj_) {
            return true;
        }
        return false;
    };

private:
    T* observable_obj_;
    ControlBlockBase* cb_;
};

template <typename T, typename U>
inline bool operator==(const SharedPtr<T>& left, const SharedPtr<U>& right);

// Allocate memory only once
template <typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args) {
    ControlBlockOwning<T>* tmp = new ControlBlockOwning<T>(std::forward<Args>(args)...);
    return SharedPtr<T>{tmp};
};

// Look for usage examples in tests
template <typename T>
class EnableSharedFromThis {
public:
    SharedPtr<T> SharedFromThis();
    SharedPtr<const T> SharedFromThis() const;

    WeakPtr<T> WeakFromThis() noexcept;
    WeakPtr<const T> WeakFromThis() const noexcept;
};
