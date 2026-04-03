#include "server.h"

#include <iostream>
#include <ranges>

// ---------------------------------------------------------------------------
// HTTP request handler 
// ---------------------------------------------------------------------------
template<class Body, class Allocator>
boost::beast::http::message_generator
handle_request(boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>>&& req)
{
    auto const bad_request = [&req](boost::beast::string_view why) {
        boost::beast::http::response<boost::beast::http::string_body> res{
            boost::beast::http::status::bad_request, req.version()};
        res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(boost::beast::http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = std::string(why);
        res.prepare_payload();
        return res;
    };

    auto const not_found = [&req](boost::beast::string_view target) {
        boost::beast::http::response<boost::beast::http::string_body> res{
            boost::beast::http::status::not_found, req.version()};
        res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(boost::beast::http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = "The resource '" + std::string(target) + "' was not found.";
        res.prepare_payload();
        return res;
    };

    auto const server_error = [&req](boost::beast::string_view what) {
        boost::beast::http::response<boost::beast::http::string_body> res{
            boost::beast::http::status::internal_server_error, req.version()};
        res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(boost::beast::http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = "An error occurred: '" + std::string(what) + "'";
        res.prepare_payload();
        return res;
    };

    if (req.method() != boost::beast::http::verb::get
        && req.method() != boost::beast::http::verb::post)
        return bad_request("Unknown HTTP-method");

    boost::beast::http::response<boost::beast::http::string_body> res{
        boost::beast::http::status::ok, req.version()};
    res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(boost::beast::http::field::content_type, "application/json");
    res.body() = std::string("hello test");
    res.keep_alive(req.keep_alive());
    res.prepare_payload();
    return res;
}

// ---------------------------------------------------------------------------
// Client_session
// ---------------------------------------------------------------------------

// 接收 stream 所有权，在内部构造 ws
Client_session::Client_session(boost::asio::io_context& ioc, boost::beast::websocket::stream<stream_type&>&& ws)
    : m_io_context(ioc)
    , m_ws(std::move(ws))
    , m_strand(ioc.get_executor())
{
}

Client_session::~Client_session()
{
    // 析构时 ws 已经完全归我们所有，可以安全地做同步 close
    // 注意：此处不能再发起协程，只做最低限度的 socket 层面关闭
    std::cout << "Client_session destroyed for userId: " << m_userId << "\n";
    boost::system::error_code ec;
    boost::beast::get_lowest_layer(m_ws).socket().shutdown(
        boost::asio::ip::tcp::socket::shutdown_both, ec);
    boost::beast::get_lowest_layer(m_ws).socket().close(ec);
}

// 主动关闭：从外部调用，通过 strand 序列化，安全发起 ws close 握手
void Client_session::close()
{
    auto self = shared_from_this();
    boost::asio::post(m_strand, [self]() {
        if (self->m_closed) return;
        self->m_closed = true;

        // 发起异步 ws close，不等结果（析构时会做 socket 层面清理）
        self->m_ws.async_close(
            boost::beast::websocket::close_code::going_away,
            [self](boost::system::error_code ec) {
                if (ec)
                    std::cerr << "ws close error [" << self->m_userId << "]: "
                              << ec.message() << "\n";
            });
    });
}

// ---------------------------------------------------------------------------
// 写队列：全部在 strand 内操作，无需额外锁
// ---------------------------------------------------------------------------

void Client_session::do_write(const char* data, std::uint32_t size)
{
    // 拷贝到 vector，解决裸指针所有权问题
    BuffData buf(data, data + size);
    boost::asio::post(m_strand, [this, self = shared_from_this(), buf = std::move(buf)]() mutable {
        if (m_closed) return;
        if (m_datas.size() >= k_max_queue) return;  // 队列满则丢弃
        m_datas.push(std::move(buf));
        if (!m_isWriting) {
            m_isWriting = true;
            write();
        }
    });
}

void Client_session::do_write(const std::string& data)
{
    boost::asio::post(m_strand, [this, self = shared_from_this(), data]() {
        if (m_closed) return;
        if (m_datas.size() >= k_max_queue) return;
        m_datas.push(data);
        if (!m_isWriting) {
            m_isWriting = true;
            write();
        }
    });
}

// write() 只在 strand 内被调用，不需要加锁
void Client_session::write()
{
    // self 传入协程和 completion handler，保证生命周期
    boost::asio::co_spawn(
        m_strand,
        [self = shared_from_this()]() -> boost::asio::awaitable<void, executor_type> {
            while (!self->m_datas.empty() && !self->m_closed) {
                auto& front = self->m_datas.front();
                co_await std::visit(
                    [&self](auto&& arg) -> boost::asio::awaitable<
                        std::tuple<boost::system::error_code, std::size_t>, executor_type>
                    {
                        using T = std::decay_t<decltype(arg)>;
                        if constexpr (std::is_same_v<T, std::string>) {
                            auto [ec,_] = co_await self->m_ws.async_write(
                                boost::asio::buffer(arg),
                                boost::asio::as_tuple(
                                    boost::asio::cancel_after(std::chrono::seconds(10))));
                            if (ec) {
                                // 抛出异常由 completion handler 处理，带 userId 便于上层识别
                                throw boost::system::system_error{ec, self->m_userId};
                            }
                        } else {
                            // BuffData = vector<char>
                            auto [ec,_] =co_await self->m_ws.async_write(
                                boost::asio::buffer(arg),
                                boost::asio::as_tuple(
                                    boost::asio::cancel_after(std::chrono::seconds(10))));
                            if (ec) {
                                // 抛出异常由 completion handler 处理，带 userId 便于上层识别
                                throw boost::system::system_error{ec, self->m_userId};
                            }
                        }

                    },
                    front);
                self->m_datas.pop();
            }
            self->m_isWriting = false;
        },
        // completion handler 也持有 self，确保协程帧销毁后对象仍然存活
        [self = shared_from_this()](std::exception_ptr e) {
            self->m_isWriting = false;
            if (e) {
                try { std::rethrow_exception(e); }
                catch (std::exception& ex) {
                    std::cerr << "write error [" << self->m_userId << "]: "
                              << ex.what() << "\n";
                }
            }
        });
}

void Client_session::setUserId(std::string userId)
{
    m_userId = std::move(userId);
}

// ---------------------------------------------------------------------------
// do_read
// ---------------------------------------------------------------------------
boost::asio::awaitable<void, executor_type> Client_session::do_read()
{
    auto cs = co_await boost::asio::this_coro::cancellation_state;
    boost::beast::flat_buffer buffer;

    while (!cs.cancelled()) {
        auto [ec, size] = co_await m_ws.async_read(buffer, boost::asio::as_tuple);

        if (ec == boost::beast::websocket::error::closed) {
            std::cout << "WebSocket closed by client [" << m_userId << "]\n";
            co_return;
        }

        if (ec == boost::asio::error::operation_aborted) {
            std::cout << "WebSocket read aborted [" << m_userId << "]\n";
            // 尝试发送 close 帧，忽略错误
            co_await m_ws.async_close(
                boost::beast::websocket::close_code::going_away,
                boost::asio::as_tuple(boost::asio::cancel_after(std::chrono::seconds(5))));
            co_return;
        }

        if (ec) {
            // 其他错误（EOF 等）向上抛，由 run_session 的 completion handler 清理 session
            throw boost::system::system_error(ec, m_userId);
        }

        if (m_ws.got_text()) {
            std::cout << "recv [" << m_userId << "]: "
                      << boost::beast::buffers_to_string(buffer.data()) << "\n";
            if(m_on_string_message) m_on_string_message(boost::beast::buffers_to_string(buffer.data()));
        } else {
            std::shared_ptr<char> data = std::make_shared<char>(size);
            std::memcpy(data.get(), buffer.data().data(), size);
            if(m_on_binary_message) m_on_binary_message(data);
        }

        buffer.consume(buffer.size());
    }
    co_return;
}

// ---------------------------------------------------------------------------
// Server
// ---------------------------------------------------------------------------

Server::Server(std::uint16_t port, std::uint16_t threads)
    : m_io_context(threads), m_port(port)
      , m_task_group( std::make_shared<Task_group>(m_io_context.get_executor()) )
{
    m_threads.resize(threads);
}

Server::~Server()
{
    m_io_context.stop();
    for (auto& th : m_threads) {
        if (th.joinable())
            th.join();
    }
}

void Server::on_session_error(const std::string& userId)
{
    auto it = m_sessions.find(userId);
    if (it == m_sessions.end()) return;

    it->second->close();
    m_sessions.erase(it);

    std::cout << "Session removed [" << userId << "], active=" << m_sessions.size() << "\n";
}

boost::asio::awaitable<void, executor_type> Server::run_session(stream_type stream)
{
    boost::beast::flat_buffer buffer;

    co_await boost::asio::this_coro::reset_cancellation_state(
        boost::asio::enable_total_cancellation(),
        boost::asio::enable_terminal_cancellation());
    co_await boost::asio::this_coro::throw_if_cancelled(false);

    auto cs = co_await boost::asio::this_coro::cancellation_state;

    while (!cs.cancelled()) {
        stream.expires_after(std::chrono::seconds(30));
        boost::beast::http::request_parser<boost::beast::http::string_body> parser;
        parser.body_limit(1024 * 1024 * 100);

        auto [ec, _] = co_await boost::beast::http::async_read(
            stream, buffer, parser, boost::asio::as_tuple);
        if (ec == boost::beast::http::error::end_of_stream)
            co_return;

        if (boost::beast::websocket::is_upgrade(parser.get())) {
            boost::beast::get_lowest_layer(stream).expires_never();

            // 把 stream 所有权完整地转移给 ws，再转移给 Client_session
            // 这样 run_session 协程帧退出后不影响 session 生命周期
            boost::beast::websocket::stream<stream_type&> ws(stream);
            ws.set_option(boost::beast::websocket::stream_base::timeout::suggested(
                boost::beast::role_type::server));
            ws.set_option(boost::beast::websocket::stream_base::decorator(
                [](boost::beast::websocket::response_type& res) {
                    res.set(boost::beast::http::field::server,
                            std::string(BOOST_BEAST_VERSION_STRING));
                }));

            auto req = parser.release();
            co_await ws.async_accept(req);

            // 解析 query string
            std::string token;
            size_t query_pos = req.target().find('?');
            if (query_pos != std::string::npos) {
                std::string query_string = req.target().substr(query_pos + 1);
                std::istringstream query_stream(query_string);
                std::string param;
                while (std::getline(query_stream, param, '&')) {
                    size_t eq_pos = param.find('=');
                    if (eq_pos != std::string::npos && param.substr(0, eq_pos) == "token") {
                        token = param.substr(eq_pos + 1);
                        break;
                    }
                }
            }

            if (!token.empty()) {
                // stream 所有权移入 Client_session
                auto session = std::make_shared<Client_session>(
                    m_io_context, std::move(ws));
                session->setUserId(token);
                if(m_sessions.contains(token)) {
                    std::cout << "Duplicate token: " << token << ", closing new session\n";
                    m_sessions[token]->close();  // 先关闭旧 session
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));  // 等待旧 session 清理
                }
                m_sessions[token] = session;
                co_await session->do_read();
                // do_read 正常返回（客户端主动关闭）时在这里清理
                on_session_error(token);
            }
            co_return;
        }

        auto res = handle_request(parser.release());
        bool keep = res.keep_alive();
        co_await boost::beast::async_write(stream, std::move(res));
        if (!keep) co_return;
        buffer.consume(buffer.size());
    }

    if (stream.socket().is_open())
        stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_send);
    co_return;
}

boost::asio::awaitable<void, executor_type> Server::listen(
    boost::asio::ip::tcp::endpoint endpoint)
{
    auto cs       = co_await boost::asio::this_coro::cancellation_state;
    auto executor = co_await boost::asio::this_coro::executor;
    auto acceptor = boost::asio::ip::tcp::acceptor::rebind_executor<executor_type>::other(
        executor, endpoint);

    co_await boost::asio::this_coro::reset_cancellation_state(
        boost::asio::enable_total_cancellation());

    while (!cs.cancelled()) {
        auto socket_executor = boost::asio::make_strand(executor.get_inner_executor());
        auto [ec, socket]    = co_await acceptor.async_accept(
            socket_executor, boost::asio::as_tuple);
        if (ec) throw boost::system::system_error{ec};

        boost::asio::co_spawn(
            boost::asio::make_strand(m_io_context),
            this->run_session(stream_type{std::move(socket)}),
            m_task_group->adapt([this](std::exception_ptr e) {
                if (!e) return;
                try { std::rethrow_exception(e); }
                catch (std::exception& ex) {
                    // 从异常信息里拿 userId（格式 "userId: error message"）
                    std::string sv = ex.what();
                    std::string userId;
                    for (auto&& part : sv | std::views::split(':')) {
                        for (char c : part) userId.push_back(c);
                        break;
                    }
                    on_session_error(userId);
                    std::cerr << "session error: " << ex.what() << "\n";
                }
            }));
    }
    co_return;
}

void Server::run()
{
    auto const endpoint = boost::asio::ip::tcp::endpoint(
        boost::asio::ip::make_address("0.0.0.0"),
        static_cast<unsigned short>(m_port));

    boost::asio::co_spawn(
        boost::asio::make_strand(m_io_context),
        this->listen(endpoint),
        m_task_group->adapt([](std::exception_ptr e) {
            if (!e) return;
            try { std::rethrow_exception(e); }
            catch (std::exception& ex) {
                std::cerr << "listener error: " << ex.what() << "\n";
            }
        }));

    for (auto& t : m_threads)
        t = std::jthread([this] { m_io_context.run(); });
}