#include "http_client.h"

#include <iostream>

int main()
{
    HttpClient client;

    HttpRequest req("192.168.1.101", "6677", "/",
                    [](const char *data, std::size_t size)
                    {
                        std::cout << "Response received: " << std::string(data, size) << "\n";
                    });

    std::vector<std::jthread> workers;
    for (int i = 0; i < 1; ++i)
    {
        workers.emplace_back([&client, req, i](std::stop_token stoken)
                             {
            while (!stoken.stop_requested()) {
                client.do_request(req);
                std::this_thread::sleep_for(std::chrono::seconds(5)); // 每5秒发起一次请求
            } });
    }
    // 给 io_context 线程一点时间处理，实际应用中应该有更好的同步机制
    std::this_thread::sleep_for(std::chrono::seconds(25));
    return 0;
}