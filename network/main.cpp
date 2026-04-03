#include <iostream>
#include "server.h"

int main()
{
    Server server(6677, 4);
    server.run();

    std::vector<std::jthread> workers;
    char key;

    while (true) {
        std::cout << "input: ";
        std::cin >> key;

        if (key == 'r') {

            for(auto& sessions : server.m_sessions) {
                std::cout << "active session userId: " << sessions.first << "\n";
                for (int i = 0; i < 3; i++) {
                    workers.emplace_back([ weak = std::weak_ptr(sessions.second), i](std::stop_token stoken) {
                    while (!stoken.stop_requested()) {
                        auto session = weak.lock();
                        if (!session) {
                            // std::cout << "session gone, worker " << i << " exits\n";
                            break;  // session 已销毁，线程主动退出
                        }
                        session->do_write("test" + std::to_string(i));
                        session.reset();  // ← 立刻释放，不等到下次循环开头

                        // 给 io_context 线程一点时间处理，避免空转
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    }
                });
            }
            }
        }

        if (key == 't') {
            server.on_session_error("abc123");
        }

        if (key == 'q') {
            // jthread 析构自动 request_stop + join，workers 里的线程会退出
            // 之后 server 析构，io_context stop
            return 0;
        }
    }
}