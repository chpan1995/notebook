#include <cstdint>
#include <grpcpp/completion_queue.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/server.h>
#include <grpcpp/server_context.h>
#include <grpcpp/support/server_callback.h>
#include <iostream>

#include "helloworld.grpc.pb.h"

class HWServiveImpl : public helloworld::Greeter::CallbackService {
public:
  // ServerBidiReactor 是被用来处理双向流式 RPC 的类
  grpc::ServerBidiReactor<helloworld::HelloRequest, helloworld::HelloReply> *
  SayHelloBindStream(grpc::CallbackServerContext *context) override {
    class Reactor : public grpc::ServerBidiReactor<helloworld::HelloRequest,
                                                   helloworld::HelloReply> {
    public:
      Reactor() { StartRead(&m_request); }
      void OnReadDone(bool ok) override {
        if (!ok) {
          // 如果读取操作失败，通常是因为客户端取消了请求或连接断开
          // 在这种情况下，我们可以选择结束这个流或继续等待下一个请求
          std::cout << "onReadDone Cancelled" << std::endl;
          return Finish(grpc::Status::CANCELLED);
        }
        std::cout << "Received request: " << m_request.name() << std::endl;
        m_reply.set_message("hello -> " + m_request.name());
        StartWrite(&m_reply);
      }
      void OnWriteDone(bool ok) override {
        if (!ok) {
          // 如果写操作失败，通常是因为客户端取消了请求或连接断开
          // 在这种情况下，我们可以选择结束这个流或继续等待下一个请求
          // 这里我们选择结束流
          std::cout << "onWriteDone Cancelled" << std::endl;
          return Finish(grpc::Status::CANCELLED);
        }
        StartRead(&m_request);
      }
      void OnDone() override { delete this; }

    private:
      helloworld::HelloRequest m_request;
      helloworld::HelloReply m_reply;
    };
    return new Reactor;
  }

  grpc::ServerUnaryReactor *SayHello(grpc::CallbackServerContext *context,
                                     const helloworld::HelloRequest *request,
                                     helloworld::HelloReply *reply) override {
    std::string prefix("Hello -> ");
    reply->set_message(prefix + request->name());
    grpc::ServerUnaryReactor *reactor = context->DefaultReactor();
    reactor->Finish(grpc::Status::OK);
    return reactor;
  }

  grpc::ServerWriteReactor<helloworld::HelloReply>* SayHelloStreamReply(
      grpc::CallbackServerContext* context, const ::helloworld::HelloRequest* request) override {
    class Reactor : public grpc::ServerWriteReactor<helloworld::HelloReply> {
    public:
      Reactor(helloworld::HelloRequest request) : m_request(request) {
        // 启动写操作
        m_reply.set_message("hello -> " + m_request.name());
        StartWrite(&m_reply);
      }

        void OnWriteDone(bool ok) override {
            if (!ok) {
            // 如果写操作失败，通常是因为客户端取消了请求或连接断开
            // 在这种情况下，我们可以选择结束这个流或继续等待下一个请求
            std::cout << "onWriteDone Cancelled" << std::endl;
            // 这里我们选择结束流会进入 OnDone 方法
            return Finish(grpc::Status::CANCELLED);
            }
            m_count++;
            if (m_count < 10) {
                // 如果计数小于10，继续发送消息
                m_reply.set_message("hello -> " + m_request.name() + " " + std::to_string(m_count));
                std::cout << "send " << m_reply.message() << std::endl;
                StartWrite(&m_reply);
            } else {
                // 如果计数达到10，结束流
                std::cout << "onWriteDone Finish" << std::endl;
                // 这里我们选择结束流会进入 OnDone 方法
                Finish(grpc::Status::OK);
            }
        }

      // OnDone 方法在流结束时调用
      // Finish 方法调用后会自动调用 OnDone
      void OnDone () override {
        // 结束流
        delete this;
      }
      private:
        helloworld::HelloRequest m_request;
        helloworld::HelloReply m_reply;
        std::uint8_t m_count = 0;
      };
       return new Reactor(*request);
    }; 

    grpc::ServerReadReactor<helloworld::HelloRequest>* SayHelloStream(
      grpc::CallbackServerContext* context, helloworld::HelloReply* reply) override {
    class Reactor : public grpc::ServerReadReactor<helloworld::HelloRequest> {
    public:
      Reactor(helloworld::HelloReply* reply) : m_reply(reply) {
        StartRead(&m_request);
      }
      void OnReadDone(bool ok) override {
        if (!ok) {
          // 如果读取操作失败或客户端关闭流，通常是因为客户端取消了请求或连接断开
          // 在这种情况下，我们可以选择结束这个流或继续等待下一个请求
          m_reply->set_message("收到所有请求，回复内容");
          std::cout << "onReadDone Cancelled" << std::endl;
          // 这里我们选择结束流 会自动发送回复内容
          return Finish(grpc::Status::OK);
        }
        std::cout << "Received request: " << m_request.name() << std::endl;
        StartRead(&m_request);
      }
      void OnDone() override {
        // 流结束时调用
        std::cout << "Stream finished" << std::endl;
        delete this;
      }
    private:
      helloworld::HelloRequest m_request;
      helloworld::HelloReply* m_reply;
    };
    return new Reactor(reply);
  }
};

int main(int argc, char **argv) {
  HWServiveImpl hws;

  grpc::ServerBuilder builder;
  builder.AddListeningPort("0.0.0.0:6666", grpc::InsecureServerCredentials());
  builder.RegisterService(&hws);
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  server->Wait();
  return 0;
}