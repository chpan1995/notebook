#pragma once

#include "../Gcorotask.h"

namespace GCoro::detail
{

template<typename Promise>
inline bool TaskAwaiterBase<Promise>::await_ready() const noexcept {
    return mAwaitedCoroutine && mAwaitedCoroutine.done();
}

template<typename Promise>
inline void TaskAwaiterBase<Promise>::await_suspend(std::coroutine_handle<> awaitingCoroutine) noexcept {
    if (!mAwaitedCoroutine) {
        std::cout << "QCoro::Task: Awaiting a default-constructed or a moved-from QCoro::Task<> - this will hang forever!" << std::endl;
        return;
    }

    mAwaitedCoroutine.promise().addAwaitingCoroutine(awaitingCoroutine);
}
template<typename Promise>
inline TaskAwaiterBase<Promise>::TaskAwaiterBase(std::coroutine_handle<Promise> awaitedCoroutine)
    : mAwaitedCoroutine(awaitedCoroutine) {}

}

