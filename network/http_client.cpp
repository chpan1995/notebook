#include "http_client.h"

#include <iostream>

HttpClient::HttpClient() : m_requestChannel(m_io_context.get_executor()
    ,1024 /*std::numeric_limits<std::size_t>::max()缓存区大小*/) {

    boost::asio::co_spawn(m_io_context,this->do_session(),[](std::exception_ptr e){
         if (!e) return;
            try { std::rethrow_exception(e); }
            catch (std::exception& ex) {
                std::cerr << "do_session error: " << ex.what() << "\n";
            }
    });

    m_th = std::jthread([this] {
        auto work = boost::asio::make_work_guard(m_io_context);
        m_io_context.run();
    });
}

HttpClient::~HttpClient() {
    m_io_context.stop();
    if (m_th.joinable())
        m_th.join();
}

void  HttpClient::do_request(const HttpRequest& req) {
    m_requestChannel.async_send(boost::system::error_code{}, req,
        [](boost::system::error_code ec) {
            if (ec) std::cerr << "Failed to send request: " << ec.message() << "\n";
        });
}

boost::asio::awaitable<void> HttpClient::do_session() {
    auto cs = co_await boost::asio::this_coro::cancellation_state;
    while (!cs.cancelled())
    {
        auto [ec, req] = co_await m_requestChannel.async_receive(boost::asio::as_tuple);
        if (ec) {
            std::cerr << "Failed to receive request: " << ec.message() << "\n";
            continue; // 继续等待下一个请求
        }
        auto executor = co_await boost::asio::this_coro::executor;
        auto resolver = boost::asio::ip::tcp::resolver{ executor };
        auto stream   = boost::beast::tcp_stream{ executor };
        auto [ec_resolve, results] = co_await resolver.async_resolve(req.host, req.port, boost::asio::as_tuple);
        if (ec_resolve) {
            std::cerr << "Failed to resolve host: " << ec_resolve.message() << "\n";
            continue; // 继续等待下一个请求
        }
        stream.expires_after(std::chrono::seconds(30));
        // 不手动拿到ec，让异常机制处理连接错误，简化代码 失败时会抛出异常，带有错误信息和请求目标（host:port），便于上层识别和调试
        auto [ec_connect,t] = co_await stream.async_connect(results, boost::asio::as_tuple);
        if (ec_connect) {
            std::cerr << "Failed to connect: " << ec_connect.message() << "\n";
            continue; // 继续等待下一个请求
        }
        switch (req.method)
        {
            case HttpRequest::HttpMethod::GET: {
                boost::beast::http::request<boost::beast::http::string_body> http_req{
                    boost::beast::http::verb::get, req.target, req.version };
                http_req.set(boost::beast::http::field::host, req.host);
                http_req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
                for (const auto& [key, value] : req.headers) {
                    http_req.set(key, value);
                }
                http_req.prepare_payload();
                auto [ec_write, _] = co_await boost::beast::http::async_write(stream, http_req, boost::asio::as_tuple);
                if (ec_write) {
                    std::cerr << "Failed to write request: " << ec_write.message() << "\n";
                    continue; // 继续等待下一个请求
                }
            }
                break;
            case HttpRequest::HttpMethod::POST: {
                boost::beast::http::request<boost::beast::http::string_body> http_req{
                    boost::beast::http::verb::post, req.target, req.version };
                http_req.set(boost::beast::http::field::host, req.host);
                http_req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
                for (const auto& [key, value] : req.headers) {
                    http_req.set(key, value);
                }
                http_req.body() = req.body;
                http_req.prepare_payload();
                auto [ec_write, _] = co_await boost::beast::http::async_write(stream, http_req, boost::asio::as_tuple);
                if (ec_write) {
                    std::cerr << "Failed to write request: " << ec_write.message() << "\n";
                    continue; // 继续等待下一个请求
                }
            }
                 break;
            default:
                break;
        }
        boost::beast::flat_buffer buffer;
        boost::beast::http::response<boost::beast::http::string_body> http_res;
        auto [ec_read, _] = co_await boost::beast::http::async_read(stream, buffer, http_res, boost::asio::as_tuple);
        if (ec_read) {
            std::cerr << "Failed to read response: " << ec_read.message() << "\n";
            continue; // 继续等待下一个请求
        }
        boost::beast::error_code shutdown_ec;
        stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, shutdown_ec);
        if(shutdown_ec && shutdown_ec != boost::beast::errc::not_connected)
            std::cerr << "Failed to shutdown stream: " << shutdown_ec.message() << "\n";

            // 调用回调函数处理响应，传入响应体数据和大小
        req.callback(http_res.body().data(), http_res.body().size());
    }
    std::cout << "do_session exiting...\n";
}