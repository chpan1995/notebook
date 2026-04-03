#include "websocket_client.h"

#include <iostream>

WebsocketClient::WebsocketClient(std::string host,std::string port,std::string target)
: m_host(host),m_port(port),m_target(target),m_timer(m_io_context)
  , m_requestChannel(m_io_context.get_executor(),1024)
  , m_task_group(std::make_shared<Task_group>(m_io_context.get_executor())) {
    m_th = std::jthread([this] {
        auto work = boost::asio::make_work_guard(m_io_context);
        m_io_context.run();
    });
}

WebsocketClient::~WebsocketClient() {
    m_io_context.stop();
    if (m_th.joinable())
        m_th.join();
}

void WebsocketClient::run() {
    boost::asio::co_spawn(m_io_context,[this]() -> boost::asio::awaitable<void> {
        co_await boost::asio::this_coro::reset_cancellation_state(
            boost::asio::enable_total_cancellation(), boost::asio::enable_terminal_cancellation());
        auto cs = co_await boost::asio::this_coro::cancellation_state;
        while(!cs.cancelled()) {
            auto executor = co_await boost::asio::this_coro::executor;
            auto resolver = boost::asio::ip::tcp::resolver{ executor };
            m_ws = std::make_shared<boost::beast::websocket::stream<boost::beast::tcp_stream>>(executor);
            const auto results = co_await resolver.async_resolve(m_host, m_port);
            boost::beast::get_lowest_layer(*m_ws).expires_after(std::chrono::seconds(30));

            auto ep = co_await boost::beast::get_lowest_layer(*m_ws).async_connect(results);
            auto host = m_host + ':' + std::to_string(ep.port());

            boost::beast::get_lowest_layer(*m_ws).expires_never();

            m_ws->set_option(boost::beast::websocket::stream_base::timeout::suggested(
                boost::beast::role_type::client));
            
            m_ws->set_option(boost::beast::websocket::stream_base::decorator(
                [](boost::beast::websocket::request_type& req)
                {
                    // 设置一些请求头
                    req.set(
                    boost::beast::http::field::user_agent,
                    std::string(BOOST_BEAST_VERSION_STRING));
                })
            );
            
            // 失败默认抛出异常
            co_await m_ws->async_handshake(host, "/" + m_target);
            session_write();
            while (!cs.cancelled())
            {
                boost::beast::flat_buffer buffer;
                co_await m_ws->async_read(buffer);
                
                std::string msg = boost::beast::buffers_to_string(buffer.data());
                std::cout << "Received message: " << msg << "\n";
                buffer.consume(buffer.size());
                // TODO Callback
            }
            
        }
    },m_task_group->adapt([this](std::exception_ptr e){
        if (!e) return;
        try
        {
            std::rethrow_exception(e);
        }
        catch(const std::exception& e)
        {
            std::cerr << "WebsocketClient read error: " << e.what() << "\n";
        }
        boost::asio::post(m_io_context,[this]{
            // 可能会执行两次 这里read会执行一次，下面的write也会执行一次;
            release();
            m_timer.expires_after(std::chrono::seconds(3));
            m_timer.async_wait([this](boost::system::error_code ec){
                if(ec == boost::asio::error::operation_aborted) {
                    std::cout << "WebsocketClient timer cancelled\n";
                    return;
                }
                // 重新连接
                run();
            });
        });
    }));
}

void WebsocketClient::do_write(const std::string& data) {
    if(m_ws && m_ws->is_open()) {
        m_requestChannel.async_send(boost::system::error_code(),data
            ,[](boost::system::error_code ec){
            if (ec) std::cerr << "Failed to do_write: " << ec.message() << "\n";
        });
    }
}

void WebsocketClient::do_write(char* data,std::uint32_t size) {
    if(m_ws && m_ws->is_open()) {
        BuffData buffdata(data,data + size);
        m_requestChannel.async_send(boost::system::error_code(),buffdata
            ,[](boost::system::error_code ec){
            if (ec) std::cerr << "Failed to do_write: " << ec.message() << "\n";
        });
    }
}

void WebsocketClient::session_write() {
    boost::asio::co_spawn(m_io_context,[this]() -> boost::asio::awaitable<void> {
        if(m_ws && m_ws->is_open()) {
            auto cs = co_await boost::asio::this_coro::cancellation_state;
            co_await boost::asio::this_coro::reset_cancellation_state(
                boost::asio::enable_total_cancellation(), boost::asio::enable_terminal_cancellation());
            while(!cs.cancelled()) {
                auto [ec, req] = co_await m_requestChannel.async_receive(boost::asio::as_tuple);
                if (ec) {
                    std::cerr << "Failed to receive request: " << ec.message() << "\n";
                    continue; // 继续等待下一个请求
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // 模拟处理请求的时间
                co_await std::visit(
                    [this](auto&& arg) -> boost::asio::awaitable<void>
                    {
                        using T = std::decay_t<decltype(arg)>;
                        if constexpr (std::is_same_v<T, std::string>) {
                            auto [ec,_] = co_await m_ws->async_write(
                                boost::asio::buffer(arg),
                                boost::asio::as_tuple(
                                    boost::asio::cancel_after(std::chrono::seconds(10))));
                            if (ec) {
                                throw boost::system::system_error{ec};
                            }
                        } else {
                            auto [ec,_] =co_await m_ws->async_write(
                                boost::asio::buffer(arg),
                                boost::asio::as_tuple(
                                    boost::asio::cancel_after(std::chrono::seconds(10))));
                            if (ec) {
                                throw boost::system::system_error{ec};
                            }
                        }

                    },req);
            }
        }
    },m_task_group->adapt([this](std::exception_ptr e){
            if (!e) return;
            try
            {
                std::rethrow_exception(e);
            }
            catch(const std::exception& e)
            {
                std::cerr << "WebsocketClient write error: " << e.what() << "\n";
            }
            boost::asio::post(m_io_context,[this]{
                release();
                m_timer.expires_after(std::chrono::seconds(3));
                m_timer.async_wait([this](boost::system::error_code ec){
                    if(ec == boost::asio::error::operation_aborted) {
                        std::cout << "WebsocketClient timer cancelled\n";
                        return;
                    }
                    // 重新连接
                    run();
            });
            });
    }));
}

void WebsocketClient::release(bool stop_io) {
    boost::asio::co_spawn(m_io_context,[this,stop_io]()->boost::asio::awaitable<void>{
        if(m_ws && m_ws->is_open()) {
            boost::system::error_code ec;
                boost::beast::get_lowest_layer(*m_ws).socket().shutdown(
            boost::asio::ip::tcp::socket::shutdown_both, ec);
            boost::beast::get_lowest_layer(*m_ws).socket().close(ec);
        }
        // 释放协程
        m_task_group->emit(boost::asio::cancellation_type::terminal);
        auto [ec] = co_await m_task_group->async_wait(
            boost::asio::as_tuple(boost::asio::cancel_after(std::chrono::seconds(3))));
        if(ec) {
            // 
            std::cerr << "Failed to wait for task_group cancellation: " << ec.message() << "\n";
        }
        std::cout << "WebsocketClient released\n";
        if(stop_io) {
            m_io_context.stop();
        }
    },boost::asio::detached);
}