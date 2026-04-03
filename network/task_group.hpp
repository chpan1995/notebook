#pragma once

#include <boost/asio.hpp>
#include <list>
#include <mutex>
#include <iostream>

class Task_group : public std::enable_shared_from_this<Task_group>
{
public:
    Task_group(boost::asio::any_io_executor exec)
        : m_cv{ std::move(exec), boost::asio::steady_timer::time_point::max() } {};
    Task_group(const Task_group&) = delete;
    Task_group& operator=(const Task_group&) = delete;

    template <typename CompletionToken>
    auto adapt(CompletionToken&& completion_token) {
        auto lg = std::lock_guard{m_mtx};
        auto cs = m_css.emplace(m_css.end());

        class remover
        {
            std::shared_ptr<Task_group> tg_;
            decltype(m_css)::iterator cs_;

        public:
            remover(
                std::shared_ptr<Task_group> tg,
                decltype(m_css)::iterator cs)
                : tg_{ tg }
                , cs_{ cs }
            {
            }

            remover(remover&& other) noexcept
                : tg_{ std::exchange(other.tg_, nullptr) }
                , cs_{ other.cs_ }
            {
            }

            ~remover()
            {
                if(tg_)
                {
                    auto lg = std::lock_guard{ tg_->m_mtx };
                    if(tg_->m_css.erase(cs_) == tg_->m_css.end())
                        tg_->m_cv.cancel();
                }
            }
        };
        return boost::asio::bind_cancellation_slot(
            cs->slot(),
            boost::asio::consign(std::forward<CompletionToken>(completion_token),
                remover{ shared_from_this(), cs }));
    }

    inline void emit(boost::asio::cancellation_type type) {
        auto lg = std::lock_guard{m_mtx};
        for (auto &cs : m_css)
            cs.emit(type);
    }

    template< typename CompletionToken =
        boost::asio::default_completion_token_t<boost::asio::any_io_executor>>
    auto
    async_wait(
        CompletionToken&& completion_token =
            boost::asio::default_completion_token_t<boost::asio::any_io_executor>{})
    {
        return boost::asio::
            async_compose<CompletionToken, void(boost::system::error_code)>(
                [this, scheduled = false](
                    auto&& self, boost::system::error_code ec = {}) mutable
                {
                    if(!scheduled)
                        self.reset_cancellation_state(boost::asio::enable_total_cancellation());

                    if(!self.cancelled() && ec == boost::asio::error::operation_aborted)
                        ec = {};

                    {
                        auto lg = std::lock_guard{ m_mtx };

                        if(!m_css.empty() && !ec)
                        {
                            scheduled = true;
                            return m_cv.async_wait(std::move(self));
                        }
                    }
                    // 让async_compose构造完成后再调用completion handler，避免在构造过程中调用handler导致的潜在问题
                    if(!std::exchange(scheduled, true))
                        return boost::asio::post(boost::asio::append(std::move(self), ec));

                    self.complete(ec);
                },
                completion_token,
                m_cv);
    }

    std::list<boost::asio::cancellation_signal> m_css;
    std::mutex m_mtx;
    boost::asio::steady_timer m_cv;
};