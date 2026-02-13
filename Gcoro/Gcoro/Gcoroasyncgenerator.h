#pragma once

#include "mixins_p.h"

#include <coroutine>
#include <iterator>
#include <exception>

namespace GCoro {
template <typename T>
class AsyncGenerator;

namespace detail {

template<typename T>
class AsyncGeneratorIterrator;
class AsyncGeneratorYieldOperation;
class AsyncGeneratorAdvanceOPeration;

class AsyncGeneratorPromiseBase : public AwaitTransformMixin {
public:
    AsyncGeneratorPromiseBase() noexcept = default;
    AsyncGeneratorPromiseBase(const AsyncGeneratorPromiseBase &) = delete;
    AsyncGeneratorPromiseBase& operator=(const AsyncGeneratorPromiseBase &) = delete;
    AsyncGeneratorPromiseBase& operator=(AsyncGeneratorPromiseBase &&) noexcept = default;

    ~AsyncGeneratorPromiseBase() = default;

    std::suspend_always initial_suspend() const noexcept { return {}; }

    AsyncGeneratorYieldOperation final_suspend() noexcept;

    void unhandled_exception() noexcept {
        m_exception = std::current_exception();
    }

    void return_void() noexcept {}

    bool finished() const noexcept {
        return m_currentValue == nullptr;
    }

    void rethrow_if_unhandled_exception() {
        if(m_exception) {
            std::rethrow_exception(std::move(m_exception));
        }
    }

protected:
    AsyncGeneratorYieldOperation internal_yield_value() noexcept;

private:
    friend class AsyncGeneratorYieldOperation;
    friend class AsyncGeneratorAdvanceOperation;
    friend class IteratorAwaitableBase;

    std::exception_ptr m_exception = nullptr;
    std::coroutine_handle<> m_consumerCoroutine;
protected:
    void *m_currentValue = nullptr;
};

class AsyncGeneratorYieldOperation final {
public:
    explicit AsyncGeneratorYieldOperation(std::coroutine_handle<> consumer) noexcept
        : m_consumer(consumer) {}

    bool await_ready() const noexcept { return false; }

    std::coroutine_handle<> await_suspend([[maybe_unused]] std::coroutine_handle<> producer) noexcept {
        return m_consumer;
    }

    void await_resume() noexcept {}

private:
    std::coroutine_handle<> m_consumer;
};

inline AsyncGeneratorYieldOperation AsyncGeneratorPromiseBase::final_suspend() noexcept {
    m_currentValue = nullptr;
    return internal_yield_value();
}

inline AsyncGeneratorYieldOperation AsyncGeneratorPromiseBase::internal_yield_value() noexcept {
    return AsyncGeneratorYieldOperation{m_consumerCoroutine};
}


class IteratorAwaitableBase {
protected:
    constexpr explicit IteratorAwaitableBase(std::nullptr_t) noexcept {}

    IteratorAwaitableBase(AsyncGeneratorPromiseBase &promise
        ,std::coroutine_handle<> producerCoroutine) noexcept
        : m_promise(std::addressof(promise))
        , m_producerCoroutine(producerCoroutine) {}
public:
    bool await_ready() const noexcept {
        return false;
    }

    std::coroutine_handle<> await_suspend(std::coroutine_handle<> consumerCoroutine) noexcept {
        m_promise->m_consumerCoroutine = consumerCoroutine;
        return m_producerCoroutine;
    }
protected:
    AsyncGeneratorPromiseBase *m_promise = nullptr;
    std::coroutine_handle<> m_producerCoroutine = {nullptr};
};

template <typename T>
class AsyncGeneratorPromise final : public AsyncGeneratorPromiseBase {
    using value_type = std::remove_reference_t<T>;
public:
    AsyncGeneratorPromise() noexcept = default;

    AsyncGenerator<T> get_return_object() noexcept;

    AsyncGeneratorYieldOperation yield_value(value_type &value) noexcept {
        m_currentValue = const_cast<std::remove_const_t<value_type> *>(std::addressof(value));
        return internal_yield_value();
    }

    AsyncGeneratorYieldOperation yield_value(value_type &&value) noexcept {
        return yield_value(value);
    }

    T &value() const noexcept {
        return *const_cast<value_type *>(static_cast<const value_type *>(m_currentValue));
    }
};

// 特化T为右值引用的情况，禁止co_yield右值引用类型的值，因为co_yield语句中的表达式会被当作左值处理，co_yield std::move(value)会导致value被移动后又被访问，产生未定义行为
template <typename T>
class AsyncGeneratorPromise<T &&> final : public AsyncGeneratorPromiseBase {
public:
    AsyncGeneratorPromise() noexcept = default;
    AsyncGenerator<T> get_return_object() noexcept;

    AsyncGeneratorYieldOperation yield_value(T&& value) noexcept {
        m_currentValue = std::addressof(value);
        return internal_yield_value();
    }

    T &&value() const noexcept {
        return std::move(*static_cast<T *>(m_currentValue));
    }
};

}; //  detail

template <typename T>
class AsyncGeneratorIterator final
{
    using promise_type = detail::AsyncGeneratorPromise<T>;
    using handle_type = std::coroutine_handle<promise_type>;
    class IncrementIteratorAwaitable final : public detail::IteratorAwaitableBase {
        public:
        explicit IncrementIteratorAwaitable(AsyncGeneratorIterator &iterator) noexcept
            : detail::IteratorAwaitableBase(iterator.m_coroutine.promise(), iterator.m_coroutine),
              m_iterator(iterator) {}

        AsyncGeneratorIterator<T> &await_resume() {
            if (m_promise->finished()) {
                m_iterator = AsyncGeneratorIterator<T>{nullptr};
                m_promise->rethrow_if_unhandled_exception();
            }
            return m_iterator;
        }
        private:
        AsyncGeneratorIterator<T> &m_iterator;
    };
public:
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = std::remove_reference_t<T>;
    using reference = std::add_lvalue_reference_t<T>;
    using pointer = std::add_pointer_t<value_type>;

    explicit constexpr AsyncGeneratorIterator(std::nullptr_t) noexcept {}
    explicit AsyncGeneratorIterator(handle_type coroutine) noexcept
        : m_coroutine(coroutine)
    {}

    auto operator++() noexcept {
        return IncrementIteratorAwaitable{*this};
    }

    reference operator *() const noexcept {
        return m_coroutine.promise().value();
    }

    bool operator==(const AsyncGeneratorIterator &other) const noexcept {
        return m_coroutine == other.m_coroutine;
    }

    bool operator!=(const AsyncGeneratorIterator &other) const noexcept {
        return !(*this == other);
    }
private:
    handle_type m_coroutine = {nullptr};
};

template<typename T>
class [[nodiscard]] AsyncGenerator {
public:
    using promise_type = detail::AsyncGeneratorPromise<T>;
    using iterator = AsyncGeneratorIterator<T>;

    AsyncGenerator() noexcept = default;
    explicit AsyncGenerator(promise_type &promise) noexcept
        : m_coroutine(std::coroutine_handle<promise_type>::from_promise(promise))
    {}

    AsyncGenerator(AsyncGenerator &&other) noexcept
        : m_coroutine(other.m_coroutine) {
        other.m_coroutine = nullptr;
    }

    /// Destructor.
    ~AsyncGenerator() {
        if (m_coroutine) {
            m_coroutine.destroy();
        }
    }
    
    AsyncGenerator& operator=(AsyncGenerator &&other) noexcept {
        m_coroutine = other.m_coroutine;
        other.m_coroutine = nullptr;
        return *this;
    }

    AsyncGenerator(const AsyncGenerator &) = delete;

    AsyncGenerator& operator=(const AsyncGenerator &) = delete;

    /**
    * \brief 返回一个类似迭代器的 awaitable 对象
    *
    * 返回的对象是一个可等待对象（awaitable），必须通过 co_await 才能获取
    * 一个异步迭代器，用于遍历异步生成器产生的结果。
    *
    * 该异步迭代器的使用方式类似普通迭代器，不同之处在于：
    * 对迭代器执行递增操作时，会返回一个 awaitable 对象，
    * 需要通过 co_await 才能完成递增。
    *
    * 如果生成器协程抛出异常，该异常会在这里被重新抛出。
    */

    auto begin() noexcept {
        class BeginIteratorAwaitable final : public detail::IteratorAwaitableBase {
        public:
            explicit BeginIteratorAwaitable(std::nullptr_t) noexcept
                : IteratorAwaitableBase(nullptr) {}

            explicit BeginIteratorAwaitable(std::coroutine_handle<promise_type> producerCoroutine) noexcept
                : IteratorAwaitableBase(producerCoroutine.promise(), producerCoroutine) {}
            
            bool await_ready() const noexcept {
                return m_promise == nullptr || IteratorAwaitableBase::await_ready();
            }

            iterator await_resume() {
                if (m_promise == nullptr) {
                    return iterator{nullptr};
                } else if (m_promise->finished()) {
                    m_promise->rethrow_if_unhandled_exception();
                    return iterator{nullptr};
                }
                return iterator{
                    std::coroutine_handle<promise_type>::from_promise(*static_cast<promise_type *>(m_promise))
                };
            }
        };
        return m_coroutine ? BeginIteratorAwaitable{m_coroutine} : BeginIteratorAwaitable{nullptr};
    }

    constexpr iterator end() const noexcept {
        return iterator{nullptr};
    }

    void swap(AsyncGenerator &other) noexcept {
        using std::swap;
        swap(m_coroutine, other.m_coroutine);
    }

private:
    std::coroutine_handle<promise_type> m_coroutine = {nullptr};
};

template<typename T>
void swap(AsyncGenerator<T> &arg1, AsyncGenerator<T> &arg2) noexcept {
    arg1.swap(arg2);
}

namespace detail {

template<typename T>
AsyncGenerator<T> AsyncGeneratorPromise<T>::get_return_object() noexcept {
    return AsyncGenerator<T>{*this};
}

} // detail

}; // GCoro


#define GCORO_FOREACH(var, generator) \
    if (auto && _container = (generator); false) {} else \
        for (auto _begin = co_await _container.begin(), _end = _container.end(); \
             _begin != _end; \
             co_await ++_begin) \
         if (var = *_begin; false) {} else
