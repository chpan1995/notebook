#pragma once

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/asio/experimental/concurrent_channel.hpp>

#include <list>
#include <variant>
#include <string>
#include <thread>

#include "task_group.hpp"

using BuffData = std::vector<char>;

class WebsocketClient {
public:
    explicit WebsocketClient(std::string host,std::string port,std::string target);
    ~WebsocketClient();

    void run();

    void do_write(const std::string&);

    void do_write(char*,std::uint32_t size);

    void release(bool stop_io = false);
private:
    void session_write();
private:
    boost::asio::io_context m_io_context;
    boost::asio::steady_timer m_timer;
    std::shared_ptr<boost::beast::websocket::stream<boost::beast::tcp_stream>> m_ws;

    using RequestChannel = boost::asio::experimental::concurrent_channel<
        boost::asio::io_context::executor_type,
        void(boost::system::error_code, std::variant<BuffData,std::string>)>;
    RequestChannel m_requestChannel;

    std::shared_ptr<Task_group> m_task_group;

    std::string m_host;
    std::string m_port;
    std::string m_target;
    std::jthread m_th;
};

