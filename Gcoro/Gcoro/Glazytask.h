#pragma once

#include "Gcorotask.h"

namespace GCoro
{

    template <typename T>
    class LazyTask;

    namespace detail
    {
        template <typename T>
        struct isTask<GCoro::LazyTask<T>> : std::true_type
        {
            using return_type = typename GCoro::LazyTask<T>::value_type;
        };

        template <typename T>
        class LazyTaskPromise : public TaskPromise<T>
        {
        public:
            using TaskPromise<T>::TaskPromise;

            LazyTask<T> get_return_object() noexcept;

            std::suspend_always initial_suspend() const noexcept;
        };
    }

    template <typename T = void>
    class LazyTask final : public detail::TaskBase<T, LazyTask, detail::LazyTaskPromise<T>>
    {
    public:
        using promise_type = detail::LazyTaskPromise<T>;
        using value_type = T;

        using detail::TaskBase<T, LazyTask, promise_type>::TaskBase;

        ~LazyTask() override;

        auto operator co_await() const noexcept;
    };

}; // GCoro

#include "impl/lazytask.h"