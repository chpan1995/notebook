#include "websocket_client.h"

#include <iostream>

int main() {
    WebsocketClient client("192.168.1.101", "6677", "?token=abc123");
    client.run();
    static int count = 0;
    std::vector<std::jthread> workers;
    for (int i = 0; i < 1; ++i) {
        workers.emplace_back([&client,i](std::stop_token stoken) {
            while (!stoken.stop_requested()) {
                client.do_write("hello " + std::to_string(count++));
                std::this_thread::sleep_for(std::chrono::milliseconds(2)); // 每5秒发起一次请求
                std::cout << "Sent message " << count << "\n";
            }
        });
    }
    // 给 io_context 线程一点时间处理，实际应用中应该有更好的同步机制
    // std::this_thread::sleep_for(std::chrono::seconds(25));
    char key;
    while (true) {
        std::cout << "input: ";
        std::cin >> key;
        if (key == 'q') {
            return 0;
        }
        if(key == 's') {
            workers[0].request_stop();
        }
    }
    return 0;
}