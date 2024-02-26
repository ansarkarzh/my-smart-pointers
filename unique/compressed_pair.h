#pragma once

#include <type_traits>
#include <algorithm>
#include <string>

template <class F, class S>
class CompressedPair;

template <class F, class S, bool IsSame, bool FirstEmpty, bool SecondEmpty>
struct compressed_pair_switch;

template <class F, class S>
struct compressed_pair_switch<F, S, false, false, false> {
    static const int kValue = 0;
};

template <class F, class S>
struct compressed_pair_switch<F, S, false, true, false> {
    static const int kValue = 1;
};

template <class F, class S>
struct compressed_pair_switch<F, S, false, false, true> {
    static const int kValue = 2;
};

template <class F, class S>
struct compressed_pair_switch<F, S, false, true, true> {
    static const int kValue = 3;
};

template <class F, class S>
struct compressed_pair_switch<F, S, true, true, true> {
    static const int kValue = 4;
};

template <class F, class S>
struct compressed_pair_switch<F, S, true, false, false> {
    static const int kValue = 0;
};

template <class F, class S, int Version>
class CompressedPairImp;

// Me think, why waste time write lot code, when few code do trick.
// derive nothing
template <typename F, typename S>
class CompressedPairImp<F, S, 0> {
public:
    CompressedPairImp() {
        first_ = F();
        second_ = S();
    }

    CompressedPairImp(const F& first, const S& second) : first_(first), second_(second) {
    }

    CompressedPairImp(F&& first, S&& second)
        : first_(std::move(first)), second_(std::move(second)) {
    }

    CompressedPairImp(const F& first, S&& second) : first_(first), second_(std::move(second)) {
    }

    CompressedPairImp(F&& first, const S& second) : first_(std::move(first)), second_(second) {
    }

    CompressedPairImp(const F& first) : first_(first) {
    }

    CompressedPairImp(const S& second) : second_(second) {
    }

    F& GetFirst() {
        return first_;
    }

    const F& GetFirst() const {
        return first_;
    };

    S& GetSecond() {
        return second_;
    }

    const S& GetSecond() const {
        return second_;
    };

private:
    F first_;
    S second_;
};

template <typename F, typename S>
class CompressedPairImp<F, S, 1> : private F {
public:
    CompressedPairImp() {
        second_ = S();
    }

    CompressedPairImp(const F& first, const S& second) : F(first), second_(second) {
    }

    CompressedPairImp(F&& first, S&& second) : F(std::move(first)), second_(std::move(second)) {
    }

    CompressedPairImp(const F& first, S&& second) : F(first), second_(std::move(second)) {
    }

    CompressedPairImp(F&& first, const S& second) : F(std::move(first)), second_(second) {
    }

    CompressedPairImp(const F& first) : F(first) {
    }

    CompressedPairImp(const S& second) : second_(second) {
    }

    F& GetFirst() {
        return *this;
    }

    const F& GetFirst() const {
        return *this;
    };

    S& GetSecond() {
        return second_;
    }

    const S& GetSecond() const {
        return second_;
    };

private:
    S second_;
};

template <typename F, typename S>
class CompressedPairImp<F, S, 2> : private S {
public:
    CompressedPairImp() {
        first_ = F();
    }

    CompressedPairImp(const F& first, const S& second) : first_(first), S(second) {
    }

    CompressedPairImp(F&& first, S&& second) : S(std::move(second)), first_(std::move(first)) {
    }

    CompressedPairImp(const F& first, S&& second) : first_(first), S(std::move(second)) {
    }

    CompressedPairImp(F&& first, const S& second) : first_(std::move(first)), S(second) {
    }

    CompressedPairImp(const F& first) : first_(first) {
    }

    CompressedPairImp(const S& second) : S(second) {
    }

    F& GetFirst() {
        return first_;
    }

    const F& GetFirst() const {
        return first_;
    };

    S& GetSecond() {
        return *this;
    }

    const S& GetSecond() const {
        return *this;
    };

private:
    F first_;
};

template <typename F, typename S>
class CompressedPairImp<F, S, 3> : private F, private S {
public:
    CompressedPairImp() {
    }

    CompressedPairImp(const F& first, const S& second) : F(first), S(second) {
    }

    CompressedPairImp(F&& first, S&& second) : F(std::move(first)), S(std::move(second)) {
    }

    CompressedPairImp(const F& first, S&& second) : F(first), S(std::move(second)) {
    }

    CompressedPairImp(F&& first, const S& second) : F(std::move(first)), S(second) {
    }

    CompressedPairImp(const F& first) : F(first) {
    }

    CompressedPairImp(const S& second) : S(second) {
    }

    F& GetFirst() {
        return *this;
    }

    const F& GetFirst() const {
        return *this;
    };

    S& GetSecond() {
        return *static_cast<S*>(this);
    }

    const S& GetSecond() const {
        return *static_cast<const S*>(this);
    };
};

template <typename F, typename S>
class CompressedPairImp<F, S, 4> : private F {
public:
    CompressedPairImp() {
        second_ = S();
    }

    CompressedPairImp(const F& first, const S& second) : F(first), second_(second) {
    }

    CompressedPairImp(F&& first, S&& second) : F(std::move(first)), second_(std::move(second)) {
    }

    CompressedPairImp(const F& first, S&& second) : F(first), second_(std::move(second)) {
    }

    CompressedPairImp(F&& first, const S& second) : F(std::move(first)), second_(second) {
    }

    CompressedPairImp(const F& first) : F(first) {
    }

    F& GetFirst() {
        return *this;
    }

    const F& GetFirst() const {
        return *this;
    };

    S& GetSecond() {
        return second_;
    }

    const S& GetSecond() const {
        return second_;
    };

private:
    S second_;
};

template <class F, class S>
class CompressedPair
    : private CompressedPairImp<
          F, S,
          compressed_pair_switch<F, S, std::is_same_v<F, S>,
                                 std::is_empty_v<F> && !std::is_final_v<F>,
                                 std::is_empty_v<S> && !std::is_final_v<S>>::kValue> {
private:
    typedef CompressedPairImp<
        F, S,
        compressed_pair_switch<F, S, std::is_same_v<F, S>,
                               std::is_empty_v<F> && !std::is_final_v<F>,
                               std::is_empty_v<S> && !std::is_final_v<S>>::kValue>
        Base;

public:
    CompressedPair() : Base() {
    }

    CompressedPair(const F& first, const S& second) : Base(first, second) {
    }

    CompressedPair(F&& first, S&& second) : Base(std::move(first), std::move(second)) {
    }

    CompressedPair(const F& first, S&& second) : Base(first, std::move(second)) {
    }

    CompressedPair(F&& first, const S& second) : Base(std::move(first), second_(second)) {
    }

    explicit CompressedPair(const F& first) : Base(first) {
    }

    explicit CompressedPair(const S& second) : Base(second) {
    }

    F& GetFirst() {
        return Base::GetFirst();
    }

    const F& GetFirst() const {
        return Base::GetFirst();
    }

    S& GetSecond() {
        return Base::GetSecond();
    }

    const S& GetSecond() const {
        return Base::GetSecond();
    }
};

template <class F>
class CompressedPair<F, F> : private CompressedPairImp<F, F, 4> {
private:
    typedef CompressedPairImp<
        F, F,
        compressed_pair_switch<F, F, std::is_same_v<F, F>,
                               std::is_empty_v<F> && !std::is_final_v<F>,
                               std::is_empty_v<F> && !std::is_final_v<F>>::kValue>
        Base;

public:
    CompressedPair() : Base() {
    }

    CompressedPair(const F& first, const F& second) : Base(first, second) {
    }

    CompressedPair(F&& first, F&& second) : Base(std::move(first), std::move(second)) {
    }

    CompressedPair(const F& first, F&& second) : Base(first, std::move(second)) {
    }

    CompressedPair(F&& first, const F& second) : Base(std::move(first), second_(second)) {
    }

    explicit CompressedPair(const F& first) : Base(first) {
    }

    F& GetFirst() {
        return Base::GetFirst();
    }

    const F& GetFirst() const {
        return Base::GetFirst();
    }

    F& GetSecond() {
        return Base::GetSecond();
    }

    const F& GetSecond() const {
        return Base::GetSecond();
    }
};
