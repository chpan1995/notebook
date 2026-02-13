#pragma once

namespace GCoro::detail {

template<HasAwaiterType T>
inline auto await_transform(T &&value) -> awaiter_type_t<std::remove_cvref_t<T>>
{
    using Awaiter = awaiter_type_t<std::remove_cvref_t<T>>;
    return Awaiter{std::forward<T>(value)};
}

template<Awaitable T>
inline auto && AwaitTransformMixin::await_transform(T &&awaitable) {
    return std::forward<T>(awaitable);
}

template<Awaitable T>
inline auto & AwaitTransformMixin::await_transform(T &awaitable) {
    return awaitable;
}

} // GCoro::detail