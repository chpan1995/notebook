#pragma once

#include <coroutine>
#include <concepts>
#include <utility>
#include <type_traits>

namespace GCoro::detail {

template<typename T>
concept has_await_methods = requires(T t) {
    { t.await_ready() } -> std::same_as<bool>;
    {t.await_suspend(std::declval<std::coroutine_handle<>>())};
    {t.await_resume()};
};

template<typename T>
concept has_member_operator_coawait = requires(T t) {
    // TODO: Check that result of co_await() satisfies Awaitable again
    { t.operator co_await() };
};

template<typename T>
concept has_nonmember_operator_coawait = requires(T t) {
    // TODO: Check that result of the operator satisfied Awaitable again
#if defined(_MSC_VER) && !defined(__clang__)
    // FIXME: MSVC is unable to perform ADL lookup for operator co_await and just fails to compile
    { ::operator co_await(static_cast<T &&>(t)) };
#else
    { operator co_await(static_cast<T &&>(t)) };
#endif
};

template<typename T>
concept Awaitable = detail::has_member_operator_coawait<T> ||
                    detail::has_nonmember_operator_coawait<T> ||
                    detail::has_await_methods<T>;

template<typename T>
struct awaiter_type;

template<typename T>
using awaiter_type_t = typename awaiter_type<T>::type;

template <typename T>
concept HasAwaiterType = requires {
    typename awaiter_type<std::remove_cvref_t<T>>::type;
};

class AwaitTransformMixin
{
public:
    template<HasAwaiterType T>
    auto await_transform(T &&value) -> awaiter_type_t<std::remove_cvref_t<T>>;

    // 如果T是Awaitable (including Task or LazyTask) ，只做转发
    template<Awaitable T>
    auto && await_transform(T &&awaitable);

    template<Awaitable T>
    auto &await_transform(T &awaitable);
};

} // namespace GCoro::detail

#include "impl/mixins.h"
