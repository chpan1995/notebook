#pragma once

#include "../Gcorotask.h"

namespace GCoro::detail
{

inline TaskFinalSuspend::TaskFinalSuspend(const std::vector<std::coroutine_handle<>> &awaitingCoroutines)
    : mAwaitingCoroutines(awaitingCoroutines) {}

inline bool TaskFinalSuspend::await_ready() const noexcept {
    return false;
}

template<typename Promise>
inline void TaskFinalSuspend::await_suspend(std::coroutine_handle<Promise> finishedCoroutine) noexcept {
    auto& promise = finishedCoroutine.promise();

    for (auto &awaiter : mAwaitingCoroutines) {
        awaiter.resume();
    }

    promise.derefCoroutine();
}

constexpr void TaskFinalSuspend::await_resume() const noexcept {}

} // GCoro::detail