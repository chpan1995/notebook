#pragma once

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/function.hpp>
#include <boost/asio/experimental/concurrent_channel.hpp>

#include <thread>
#include <string>
#include <unordered_map>

struct HttpRequest {

    enum class HttpMethod : std::uint8_t {
        GET,
        POST
    };

    std::string host;
    std::string port;
    std::string target;
    HttpMethod method; 
    std::string body;
    int version;
    std::unordered_map<std::string, std::string> headers;  // 自定义头部
    boost::function2<void,const char*,std::size_t> callback;

    HttpRequest() = default;

    HttpRequest(
        const std::string& h, 
        const std::string& p, 
        const std::string& t,
        boost::function2<void,const char*,std::size_t> cb,
        int v = 11)
        : host(h), port(p), target(t), callback(cb), version(v), method(HttpMethod::GET) {}

    HttpRequest(
        const std::string& h, 
        const std::string& p, 
        const std::string& t,
        const std::string& b,
        boost::function2<void,const char*,std::size_t> cb,
        int v = 11)
        : host(h), port(p), target(t), body(b), callback(cb), version(v), method(HttpMethod::POST) {}
};

class HttpClient {
public:
    explicit HttpClient();
    ~HttpClient();
public:
    void do_request(const HttpRequest& req);
private:
    boost::asio::awaitable<void> do_session();
private:
    boost::asio::io_context m_io_context;

    // send 可以从任意线程调用，内部加锁保证安全
    using RequestChannel = boost::asio::experimental::concurrent_channel<
        boost::asio::io_context::executor_type,
        void(boost::system::error_code, HttpRequest)>;
    RequestChannel m_requestChannel;
    // send 和 receive 必须在同一个 executor/strand 上调用，保证线程安全
    // boost::asio::experimental::channel<boost::asio::strand<boost::asio::io_context::executor_type>
    //     ,void(boost::system::error_code, bool)> m_requestChannel;
    std::jthread m_th;
};