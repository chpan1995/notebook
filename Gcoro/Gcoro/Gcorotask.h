#pragma once

#include "concepts_p.h"
#include "mixins_p.h"

#include <iostream>
#include <atomic>
#include <exception>
#include <variant>
#include <memory>
#include <type_traits>
#include <vector>

namespace GCoro {

template<typename T = void>
class Task;

namespace detail {

// coroutine 结束时封装的一个类
class TaskFinalSuspend {
public:
    explicit TaskFinalSuspend(const std::vector<std::coroutine_handle<>> &awaitingCoroutines);

    bool await_ready() const noexcept;

    template<typename Promise>
    void await_suspend(std::coroutine_handle<Promise> finishedCoroutine) noexcept;

    constexpr void await_resume() const noexcept;

private:
    std::vector<std::coroutine_handle<>> mAwaitingCoroutines;
};

// promise_type
class TaskPromiseBase : public AwaitTransformMixin {
public:
    // Called when the coroutine is started to decide whether it should be suspended or not.
    std::suspend_never initial_suspend() const noexcept;
    // Called when the coroutine co_returns or reaches the end of user code.
    /*!
     * This decides what should happen when the coroutine is finished.
     */
    auto final_suspend() const noexcept;

    // 在TaskAwaiter中调用，Task中 operate  co_await;
    /*
     * awaitingCoroutine 是当前的promise句柄
    */
    void addAwaitingCoroutine(std::coroutine_handle<> awaitingCoroutine);

    bool hasAwaitingCoroutine() const;

    void derefCoroutine();
    void refCoroutine();
    void destroyCoroutine();

protected:
    explicit TaskPromiseBase();
private:
    friend class TaskFinalSuspend;
    std::vector<std::coroutine_handle<>> mAwaitingCoroutines;
    // 引用计数
    std::atomic<uint32_t> mRefCount{0};
};

//! The promise_type for Task<T>
template<typename T>
class TaskPromise: public TaskPromiseBase {
public:
    explicit TaskPromise() = default;
    ~TaskPromise() = default;
    //! Constructs a Task<T> for this promise.
    Task<T> get_return_object() noexcept;
    // coroutine 内部未捕获异常时在这里自动调用
    // This method stores the exception. The exception is re-thrown when the calling
    // coroutine is resumed and tries to retrieve result of the finished coroutine that has thrown.
    void unhandled_exception();
    // co_return 时被调用并 store result of coroutine
    void return_value(T &&value) noexcept;

    void return_value(const T &value) noexcept;

    template<typename U> requires GCoro::concepts::constructible_from<T, U>
    void return_value(U &&value) noexcept;

    T &result() &;

    T &&result() &&;
private:
    // 存储结果monostate用来站位；T -> void
    std::variant<std::monostate, T, std::exception_ptr> mValue;
};

template<>
class TaskPromise<void>: public TaskPromiseBase {
public:
    explicit TaskPromise() = default;
    ~TaskPromise() = default;

    Task<void> get_return_object() noexcept;

    void unhandled_exception();

    void return_void() noexcept;

    void result();

private:
    std::exception_ptr mException;
};

// Base-class for Awaiter objects returned by the Task<T> operator co_await().
template<typename Promise>
class TaskAwaiterBase {
public:
    bool await_ready() const noexcept;
    void await_suspend(std::coroutine_handle<> awaitingCoroutine) noexcept;
protected:
    explicit TaskAwaiterBase(std::coroutine_handle<Promise> awaitedCoroutine);
    std::coroutine_handle<Promise> mAwaitedCoroutine = {};
};

template<typename T>
struct isTask : std::false_type
{
    using return_type = T;
};

template<typename T>
struct isTask<Task<T>> : std::true_type
{
    using return_type = typename Task<T>::value_type;
};

template<typename T>
constexpr bool isTask_v = isTask<T>::value;

template<typename T,template<typename> class TaskImpl,typename PromiseType>
class TaskBase {
public:
    explicit TaskBase() noexcept = default;

    explicit TaskBase(std::coroutine_handle<PromiseType> coroutine);
    //! Task cannot be copy-constructed.
    TaskBase(const TaskBase &) = delete;
    //! Task cannot be copy-assigned.
    TaskBase &operator=(const TaskBase &) = delete;

    //! The task can be move-constructed.
    TaskBase(TaskBase &&otbher) noexcept;

    //! The task can be move-assigned.
    TaskBase &operator=(TaskBase &&other) noexcept;

    //! Destructor.
    virtual ~TaskBase();

    bool isReady() const;

    auto operator co_await() const noexcept;

    // A callback to be invoked when the asynchronous task finishes.
    template<typename ThenCallback>
    requires (std::is_invocable_v<ThenCallback> || (!std::is_void_v<T> && std::is_invocable_v<ThenCallback, T>))
    auto then(ThenCallback &&callback) &; // 左值对象调用
    template<typename ThenCallback>
    requires (std::is_invocable_v<ThenCallback> || (!std::is_void_v<T> && std::is_invocable_v<ThenCallback, T>))
    auto then(ThenCallback &&callback) &&; // 右值对象调用
    template<typename ThenCallback, typename ErrorCallback>
    requires ((std::is_invocable_v<ThenCallback> || (!std::is_void_v<T> && std::is_invocable_v<ThenCallback, T>)) &&
               std::is_invocable_v<ErrorCallback, const std::exception &>)
    auto then(ThenCallback &&callback, ErrorCallback &&errorCallback) &;
    template<typename ThenCallback, typename ErrorCallback>
    requires ((std::is_invocable_v<ThenCallback> || (!std::is_void_v<T> && std::is_invocable_v<ThenCallback, T>)) &&
               std::is_invocable_v<ErrorCallback, const std::exception &>)
    auto then(ThenCallback &&callback, ErrorCallback &&errorCallback) &&;
private:
    template<typename ThenCallback, typename ... Args>
    static auto invokeCb(ThenCallback&& callback,[[maybe_unused]] Args&& ... args);

    template<typename ThenCallback, typename Arg>
    struct cb_invoke_result: std::conditional_t<
        std::is_invocable_v<ThenCallback>,
        std::invoke_result<ThenCallback>,
        std::invoke_result<ThenCallback, Arg>
    >{};
    
    template<typename ThenCallback>
    struct cb_invoke_result<ThenCallback, void>: std::invoke_result<ThenCallback> {};

    template<typename ThenCallback, typename Arg>
    using cb_invoke_result_t = typename cb_invoke_result<ThenCallback, Arg>::type;

    template<typename R, typename ErrorCallback,
        typename U = typename detail::isTask<R>::return_type>
    static auto handleException(ErrorCallback &errCb, const std::exception &exception) -> U;

    template<typename TaskT, typename ThenCallback, typename ErrorCallback, typename R = cb_invoke_result_t<ThenCallback, T>>
    static auto thenImpl(TaskT task, ThenCallback &&thenCallback, ErrorCallback &&errorCallback) -> std::conditional_t<detail::isTask_v<R>, R, TaskImpl<R>>;
    template<typename TaskT, typename ThenCallback, typename ErrorCallback, typename R = cb_invoke_result_t<ThenCallback, T>>
    static auto thenImplRef(TaskT &task, ThenCallback &&thenCallback, ErrorCallback &&errorCallback) -> std::conditional_t<detail::isTask_v<R>, R, TaskImpl<R>>;

protected:
    std::coroutine_handle<PromiseType> mCoroutine = {};
};

} // namespace detail

template<typename T>
class Task final : public detail::TaskBase<T, Task, detail::TaskPromise<T>> {
public:
    //! Promise type of the coroutine. This is required by the C++ standard.
    using promise_type = detail::TaskPromise<T>;
    //! The type of the coroutine return value.
    using value_type = T;

    // 是把基类 TaskBase 的构造函数引入派生类 Task，从而让 Task 能用与 TaskBase 相同的构造器
    //（尤其是接受 std::coroutine_handle<PromiseType> 的那个构造器）。
    using detail::TaskBase<T, Task, detail::TaskPromise<T>>::TaskBase;
};

namespace detail {

template <typename T>
concept TaskConvertible = requires (T val, TaskPromiseBase promise)
{
    promise.await_transform(val);
};

template <typename T>
struct awaitable_return_type {
    using type = std::decay_t<decltype(std::declval<T>().await_resume())>;
};

template <has_member_operator_coawait T>
struct awaitable_return_type<T> {
    using type = std::decay_t<typename awaitable_return_type<decltype(std::declval<T>().operator co_await())>::type>;
};

template<has_nonmember_operator_coawait T>
struct awaitable_return_type<T> {
    using type = std::decay_t<typename awaitable_return_type<decltype(operator co_await(std::declval<T>()))>::type>;
};

template <Awaitable Awaitable>
using awaitable_return_type_t = typename awaitable_return_type<Awaitable>::type;

template <TaskConvertible Awaitable>
using convertible_awaitable_return_type_t = typename awaitable_return_type<decltype(std::declval<TaskPromiseBase>().await_transform(Awaitable()))>::type;

}; // detail

} // GCoro

#include "impl/taskbase.h"
#include "impl/taskpromise.h"
#include "impl/taskfinalsuspend.h"
#include "impl/taskpromisebase.h"
#include "impl/taskawaiterbase.h"
