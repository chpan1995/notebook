#pragma once

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/function.hpp>

#include <string>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <list>
#include <queue>
#include <variant>
#include <type_traits>
#include <vector>

#include "task_group.hpp"

using executor_type = boost::asio::strand<boost::asio::io_context::executor_type>;
using stream_type   = typename boost::beast::tcp_stream::rebind_executor<executor_type>::other;

template <typename T>
concept CompletionToken = requires(T) {
    typename std::function<T>;
} && std::is_invocable_v<T, std::exception_ptr>;


// 用 vector 管理二进制数据所有权，避免裸指针
using BuffData = std::vector<char>;

class Client_session : public std::enable_shared_from_this<Client_session>
{
public:
    // 直接接收 stream 所有权，不再持有引用
    explicit Client_session(boost::asio::io_context& ioc,
        boost::beast::websocket::stream<stream_type&>&& ws);
    ~Client_session();

    void do_write(const char* data, std::uint32_t size);
    void do_write(const std::string& data);

    boost::asio::awaitable<void, executor_type> do_read();

    void setUserId(std::string userId);

    // 主动关闭：由外部（session 断开回调）调用，安全地发起 ws close 握手
    void close();

private:
    void write();

private:
    boost::asio::io_context& m_io_context;
    boost::beast::websocket::stream<stream_type&> m_ws;
    executor_type m_strand;

    bool m_isWriting { false };
    bool m_closed    { false };  // 防止重复关闭

    static constexpr std::size_t k_max_queue = 1024;
    std::queue<std::variant<std::string, BuffData>> m_datas;
    std::string m_userId;

    boost::function1<void,std::string> m_on_string_message;
    boost::function1<void,std::shared_ptr<char>> m_on_binary_message;
};

class Server
{
public:
    explicit Server(std::uint16_t port = 6677, std::uint16_t threads = 1);
    ~Server();
    void run();

    // session 断开时的统一清理入口
    void on_session_error(const std::string& userId);

private:
    boost::asio::awaitable<void, executor_type> run_session(stream_type stream);
    boost::asio::awaitable<void, executor_type> listen(boost::asio::ip::tcp::endpoint endpoint);

    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;

private:
    boost::asio::io_context m_io_context;
    std::uint16_t m_port;
    std::vector<std::jthread> m_threads;
    std::shared_ptr<Task_group> m_task_group;

public:
    std::unordered_map<std::string, std::shared_ptr<Client_session>> m_sessions;
    
};