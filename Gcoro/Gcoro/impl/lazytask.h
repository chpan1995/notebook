#pragma once

#include "../Glazytask.h"
#include "../Gcorotask.h"

namespace GCoro
{
    namespace detail
    {
        template <typename T>
        inline LazyTask<T> LazyTaskPromise<T>::get_return_object() noexcept
        {
            return LazyTask<T>(std::coroutine_handle<LazyTaskPromise>::from_promise(*this));
        }
        template <typename T>
        inline std::suspend_always LazyTaskPromise<T>::initial_suspend() const noexcept
        {
            return {};
        }

    } // namespace detail

    template <typename T>
    inline LazyTask<T>::~LazyTask()
    {
        if (this->mCoroutine && !this->mCoroutine.done())
        {
            std::cout << "QCoro::LazyTask destroyed before it was awaited!" << std::endl;
        }
    }

    template <typename T>
    inline auto LazyTask<T>::operator co_await() const noexcept
    {
        class TaskAwaiter : public detail::TaskAwaiterBase<promise_type>
        {
        public:
            TaskAwaiter(std::coroutine_handle<promise_type> thisTask)
                : detail::TaskAwaiterBase<promise_type>{thisTask} {}
            auto await_suspend(std::coroutine_handle<> awaitingCoroutine) noexcept
            {
                detail::TaskAwaiterBase<promise_type>::await_suspend(awaitingCoroutine);
                // Return handle to the lazy task, so that it gets automatically resumed.
                return this->mAwaitedCoroutine;
            }

            auto await_resume()
            {
                assert(this->mAwaitedCoroutine);
                if constexpr (!std::is_void_v<T>)
                {
                    return std::move(this->mAwaitedCoroutine.promise().result());
                }
                else
                {
                    this->mAwaitedCoroutine.promise().result();
                }
            }
        };
        return TaskAwaiter{this->mCoroutine};
    }
} // namespace GCoro
