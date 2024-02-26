#pragma once

#include "shared.h"

#include "sw_fwd.h"  // Forward declaration

// https://en.cppreference.com/w/cpp/memory/weak_ptr
template <typename T>
class WeakPtr {
public:
    template <typename U>
    friend class SharedPtr;

    template <typename U>
    friend class WeakPtr;
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    WeakPtr() : observable_obj_{nullptr}, cb_{nullptr} {
    }

    WeakPtr(const WeakPtr& other) {
        observable_obj_ = other.observable_obj_;
        cb_ = other.cb_;
        if (cb_) {
            cb_->IncWeakCounter();
        }
    };

    WeakPtr(WeakPtr&& other)
        : observable_obj_(std::move(other.observable_obj_)), cb_(std::move(other.cb_)) {
        other.observable_obj_ = nullptr;
        other.cb_ = nullptr;
    };

    // Demote `SharedPtr`
    // #2 from https://en.cppreference.com/w/cpp/memory/weak_ptr/weak_ptr
    WeakPtr(const SharedPtr<T>& other) {
        observable_obj_ = other.observable_obj_;
        cb_ = other.cb_;
        if (cb_) {
            cb_->IncWeakCounter();
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    WeakPtr& operator=(const WeakPtr& other) {
        WeakPtr{other}.Swap(*this);
        return *this;
    };

    WeakPtr& operator=(WeakPtr&& other) {
        WeakPtr{std::move(other)}.Swap(*this);
        return *this;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~WeakPtr() {
        if (cb_) {
            cb_->DecWeakCounter();
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    void Reset() {
        WeakPtr{}.Swap(*this);
    };

    void Swap(WeakPtr& other) noexcept {
        std::swap(observable_obj_, other.observable_obj_);
        std::swap(cb_, other.cb_);
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    size_t UseCount() const {
        if (cb_) {
            return cb_->GetCounter();
        }
        return 0;
    };

    bool Expired() const {
        if (cb_) {
            return cb_->Expired();
        }
        return true;
    };

    SharedPtr<T> Lock() const {
        if (!cb_ || Expired()) {
            return SharedPtr<T>{};
        }
        return SharedPtr<T>(*this);
    };

private:
    T* observable_obj_;
    ControlBlockBase* cb_;
};
