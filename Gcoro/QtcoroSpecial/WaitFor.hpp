#pragma once

#include "Gcoro/mixins_p.h"
#include "Gcoro/Gcorotask.h"

#include <QEventLoop>
#include <optional>
#include <exception>
#include <utility>

namespace GCoro {

namespace detail {

template <typename Awaitable>
requires GCoro::detail::TaskConvertible<Awaitable>
auto toTask(Awaitable &&future) -> GCoro::Task<GCoro::detail::convertible_awaitable_return_type_t<Awaitable>> {
    co_return co_await std::forward<Awaitable>(future);
}

struct WaitContext {
    QEventLoop loop;
    bool coroutineFinished = false;
    std::exception_ptr exception;
};

template<GCoro::detail::Awaitable Awaitable>
GCoro::Task<> runCoroutine(WaitContext &context, Awaitable &&awaitable) {
    try {
        co_await std::forward<Awaitable>(awaitable);
    } catch (...) {
        context.exception = std::current_exception();
    }
    context.coroutineFinished = true;
    context.loop.quit();
}

template<typename T, GCoro::detail::Awaitable Awaitable>
GCoro::Task<> runCoroutine(WaitContext &context, std::optional<T> &result, Awaitable &&awaitable) {
    try {
        result.emplace(co_await std::forward<Awaitable>(awaitable));
    } catch (...) {
        context.exception = std::current_exception();
    }
    context.coroutineFinished = true;
    context.loop.quit();
}

template<typename T, GCoro::detail::Awaitable Awaitable>
T waitForAwaitable(Awaitable &&awaitable) {
    WaitContext context;
    if constexpr (std::is_void_v<T>) {
        // 如果Qt的再事件循环中执行的，则在件循环恢复执行
        runCoroutine(context, std::forward<Awaitable>(awaitable));
        if (!context.coroutineFinished) {
            context.loop.exec();
        }
        if (context.exception) {
            std::rethrow_exception(context.exception);
        }
    } else {
        std::optional<T> result;
        runCoroutine(context, result, std::forward<Awaitable>(awaitable));
        if (!context.coroutineFinished) {
            context.loop.exec();
        }
        if (context.exception) {
            std::rethrow_exception(context.exception);
        }
        return std::move(*result);
    }
}

} // namespace detail


template <typename T, typename QObjectSubclass, typename Callback>
requires std::is_invocable_v<Callback> || std::is_invocable_v<Callback, T> || std::is_invocable_v<Callback, QObjectSubclass *> || std::is_invocable_v<Callback, QObjectSubclass *, T>
inline void connect(GCoro::Task<T> &&task, QObjectSubclass *context, Callback func) {
    QPointer ctxWatcher = context;
    if constexpr (std::is_same_v<T, void>) {
        task.then([ctxWatcher, func = std::move(func)]() {
            if (ctxWatcher) {
                if constexpr (std::is_member_function_pointer_v<Callback>) {
                    (ctxWatcher->*func)();
                } else {
                    func();
                }
            }
        });
    } else {
        task.then([ctxWatcher, func = std::move(func)](auto &&value) {
            if (ctxWatcher) {
                if constexpr (std::is_invocable_v<Callback, QObjectSubclass, T>) {
                    (ctxWatcher->*func)(std::forward<decltype(value)>(value));
                } else if constexpr (std::is_invocable_v<Callback, T>) {
                    func(std::forward<decltype(value)>(value));
                } else {
                    Q_UNUSED(value);
                    if constexpr (std::is_member_function_pointer_v<Callback>) {
                        (ctxWatcher->*func)();
                    } else {
                        func();
                    }
                }
            }
        });
    }
}

template <typename T, typename QObjectSubclass, typename Callback>
requires detail::TaskConvertible<T>
        && (std::is_invocable_v<Callback> || std::is_invocable_v<Callback, detail::convertible_awaitable_return_type_t<T>> || std::is_invocable_v<Callback, QObjectSubclass *> || std::is_invocable_v<Callback, QObjectSubclass *, detail::convertible_awaitable_return_type_t<T>>)
        && (!detail::isTask_v<T>)
inline void connect(T &&future, QObjectSubclass *context, Callback func) {
    auto task = detail::toTask(std::move(future));
    connect(std::move(task), context, func);
}

template<typename T>
inline T waitFor(GCoro::Task<T> &task) {
    return detail::waitForAwaitable<T>(std::forward<GCoro::Task<T>&>(task));
}

template<typename T>
inline T waitFor(GCoro::Task<T> &&task) {
    return detail::waitForAwaitable<T>(std::forward<GCoro::Task<T>>(task));
}

template<GCoro::detail::Awaitable Awaitable>
inline auto waitFor(Awaitable &&awaitable) {
    return detail::waitForAwaitable<GCoro::detail::awaitable_return_type_t<Awaitable>>(std::forward<Awaitable>(awaitable));
}

template <typename T, typename QObjectSubclass, typename Callback>
requires std::is_invocable_v<Callback> || std::is_invocable_v<Callback, T> || std::is_invocable_v<Callback, QObjectSubclass *> || std::is_invocable_v<Callback, QObjectSubclass *, T>
void connect(GCoro::Task<T> &&task, QObjectSubclass *context, Callback func);

template <typename T, typename QObjectSubclass, typename Callback>
requires detail::TaskConvertible<T>
        && (std::is_invocable_v<Callback> || std::is_invocable_v<Callback, detail::convertible_awaitable_return_type_t<T>> || std::is_invocable_v<Callback, QObjectSubclass *> || std::is_invocable_v<Callback, QObjectSubclass *, detail::convertible_awaitable_return_type_t<T>>)
        && (!detail::isTask_v<T>)
void connect(T &&future, QObjectSubclass *context, Callback func);


} // namespace QCoro