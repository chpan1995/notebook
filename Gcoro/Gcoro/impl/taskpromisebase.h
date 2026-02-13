#pragma once

#include "../Gcorotask.h"

namespace GCoro::detail
{

inline TaskPromiseBase::TaskPromiseBase()
    : mRefCount(1)
{
}

inline std::suspend_never TaskPromiseBase::initial_suspend() const noexcept {
    return {};
}

inline auto TaskPromiseBase::final_suspend() const noexcept {
    return TaskFinalSuspend{mAwaitingCoroutines};
}

inline void TaskPromiseBase::addAwaitingCoroutine(std::coroutine_handle<> awaitingCoroutine) {
    mAwaitingCoroutines.push_back(awaitingCoroutine);
}

inline bool TaskPromiseBase::hasAwaitingCoroutine() const {
    return !mAwaitingCoroutines.empty();
}

inline void TaskPromiseBase::derefCoroutine() {
    if (--mRefCount == 0) {
        destroyCoroutine();
    }
}

inline void TaskPromiseBase::refCoroutine() {
    ++mRefCount;
}

inline void TaskPromiseBase::destroyCoroutine() {
    mRefCount = 0;
    auto handle = std::coroutine_handle<TaskPromiseBase>::from_promise(*this);
    handle.destroy();
}

}
