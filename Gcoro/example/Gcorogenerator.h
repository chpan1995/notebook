#pragma once

#include <variant>
#include <exception>
#include <coroutine>
#include <type_traits>

namespace GCoro {

template <typename T>
class Generator;

namespace detail {

template <typename T>
class GeneratorPromise {
    using value_type = std::remove_reference_t<T>;
public:
    Generator<T> get_return_object();

    std::suspend_always initial_suspend() {
        return {};
    }

    std::suspend_always final_suspend() noexcept {
        mValue = nullptr;
        return {};
    }

    void unhandled_exception() {
        mException = std::current_exception();
    }

    std::suspend_always yield_value(value_type &value) {
        mValue = std::addressof(value);
        return {};
    }

    std::suspend_always yield_value(value_type &&value) {
        mValue = std::addressof(value);
        return {};
    }

    void return_void() {}

    std::exception_ptr exception() const {
        return mException;
    }

    value_type &value() {
        return *const_cast<value_type *>(static_cast<const value_type*>(mValue));
    }

    bool finished() const {
        return mValue == nullptr;
    }

    void rethrowIfException() {
        if(mException) {
            std::rethrow_exception(mException);
        }
    }

    template <typename U>
    std::suspend_never await_transform(U &&) = delete;
private:
    const void* mValue = nullptr;
    std::exception_ptr mException;
};

template <typename T>
class GeneratorIterator {
    using promise_type = GeneratorPromise<T>;
public:
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = std::remove_reference_t<T>;
    using reference = std::add_lvalue_reference_t<T>;
    using pointer = std::add_pointer_t<value_type>;

    explicit constexpr GeneratorIterator(std::nullptr_t) noexcept  {}

    GeneratorIterator operator++() {
        if(!mGeneratorCoroutine) {
            return *this;
        }
        mGeneratorCoroutine.resume();
        auto &promise = mGeneratorCoroutine.promise();
        if(promise.finished()) {
            mGeneratorCoroutine = nullptr;
            promise.rethrowIfException();
        }
        return *this;
    }

    reference operator *() const noexcept {
        return mGeneratorCoroutine.promise().value();
    }

    bool operator==(const GeneratorIterator &other) const noexcept {
        return mGeneratorCoroutine == other.mGeneratorCoroutine;
    }

    bool operator!=(const GeneratorIterator &other) const noexcept {
        return !(operator==(other));
    }
private:
    friend class GCoro::Generator<T>;

    explicit GeneratorIterator(std::coroutine_handle<promise_type> generatorCoroutine)
        : mGeneratorCoroutine(generatorCoroutine) {}

    std::coroutine_handle<promise_type> mGeneratorCoroutine{nullptr};
};

} // namespace detail

template <typename T>
class Generator {
public:
    using promise_type = detail::GeneratorPromise<T>;
    using iterator = detail::GeneratorIterator<T>;

    explicit Generator() = default;

    Generator(Generator &&other) noexcept {
        mGeneratorCoroutine = other.mGeneratorCoroutine;
        other.mGeneratorCoroutine = std::coroutine_handle<promise_type>::from_address(nullptr);
    }

    Generator(const Generator &) = delete;

    Generator &operator=(Generator &&other) noexcept {
        mGeneratorCoroutine = other.mGeneratorCoroutine;
        other.mGeneratorCoroutine = std::coroutine_handle<promise_type>::from_address(nullptr);
        return *this;
    }

    Generator &operator=(const Generator &) = delete;

    ~Generator() {
        if (mGeneratorCoroutine.address() != nullptr) {
            mGeneratorCoroutine.destroy();
        }
    }

    iterator begin() {
        mGeneratorCoroutine.resume();
        if (mGeneratorCoroutine.promise().finished()) {
            mGeneratorCoroutine.promise().rethrowIfException();
            return iterator{nullptr};
        }
        return iterator{mGeneratorCoroutine};
    }

    constexpr iterator end() const noexcept {
        return iterator{nullptr};
    }
private:
    friend detail::GeneratorPromise<T>;
    friend detail::GeneratorIterator<T>;

    explicit Generator(std::coroutine_handle<promise_type> generatorCoroutine)
    : mGeneratorCoroutine(generatorCoroutine) {}

    std::coroutine_handle<promise_type> mGeneratorCoroutine{nullptr};
};

template <typename T>
Generator<T> detail::GeneratorPromise<T>::get_return_object() {
    using handle_type = std::coroutine_handle<typename Generator<T>::promise_type>;
    return Generator<T>(handle_type::from_promise(*this));
}

} // namespace GCoro


