#include <condition_variable>
#include <grpcpp/grpcpp.h>
#include <grpcpp/support/client_callback.h>
#include <grpcpp/support/status.h>
#include <mutex>
#include <type_traits>

#include "helloworld.grpc.pb.h"

class GreeterClient {
public:
  GreeterClient(std::shared_ptr<grpc::Channel> channel)
      : m_stub(helloworld::Greeter::NewStub(channel)) {}
  // 异步调用 SayHello
  void SayHello(const std::string &name) {
    helloworld::HelloRequest request;
    request.set_name(name);
    helloworld::HelloReply reply;
    grpc::ClientContext context;
    bool done = false;
    grpc::Status status;
    std::condition_variable cv;
    std::mutex mu;

    m_stub->async()->SayHello(&context, &request, &reply,
                              [&mu, &status, &cv, &done](grpc::Status s) {
                                status = s;
                                std::unique_lock<std::mutex> lock(mu);
                                done = true;
                                cv.notify_all();
                              });

    while (!done) {
      std::unique_lock<std::mutex> lock(mu);
      cv.wait(lock, [this, &done] { return done; });
    }

    if (status.ok()) {
      std::cout << "Greeter received: " << reply.message() << std::endl;
    } else {
      std::cerr << "RPC failed: " << status.error_message() << std::endl;
    }
  }

  void SayHelloStream() {
    helloworld::HelloReply reply;
    grpc::ClientContext context;

    struct stream_control {
      // 用于控制流的状态
      bool done = false;
      grpc::Status status;
      std::condition_variable cv;
      std::mutex mu;
    };
    stream_control control;

    class ReactClientStream
        : public grpc::ClientWriteReactor<helloworld::HelloRequest> {
    public:
      ReactClientStream(stream_control &control)
          : m_count(0), m_control(control) {
        m_request.set_name("world 0");
        // StartCall();
        // StartWrite(&m_request);
      }
      // 提供一个单独的初始化方法
      void Initialize() {
        // 防止流过早结束 如果没有立即开始写入操作，gRPC
        // 可能会认为流已经完成并自动调用 OnDone()
        AddHold();
        StartCall();
        StartWrite(&m_request);
      }
      void OnWriteDone(bool ok) override {
        if (!ok) {
          std::cout << "onWriteDone Cancelled" << std::endl;
        }
        m_count++;
        if (m_count < 10) {
          m_request.set_name("world " + std::to_string(m_count));
          StartWrite(&m_request);
        } else {
          std::cout << "onWriteDone Finish" << std::endl;
          StartWritesDone();
          RemoveHold();
        }
      }
      // OnDone 方法在流结束时调用 会拿到reply服务端的回复
      void OnDone(const grpc::Status &s) override {
        m_control.done = true;
        m_control.status = std::move(s);
        std::unique_lock<std::mutex> lock(m_control.mu);
        m_control.cv.notify_all();
        std::cout << "Stream finished with status: "
                  << m_control.status.error_message() << std::endl;
        delete this;
      }

    private:
      helloworld::HelloRequest m_request;
      std::uint8_t m_count;
      stream_control &m_control;
    };
    // 创建 reactor 但不在构造函数中初始化
    auto *reactor = new ReactClientStream(control);
    // 先SayHelloStream后才能startCall 和 startWrite
    m_stub->async()->SayHelloStream(&context, &reply, reactor);
    reactor->Initialize();
    while (!control.done) {
      std::unique_lock<std::mutex> lock(control.mu);
      control.cv.wait(lock, [this, &control] { return control.done; });
    }
    if (control.status.ok()) {
      std::cout << "Greeter received: " << reply.message() << std::endl;
    } else {
      std::cerr << "RPC failed: " << control.status.error_message()
                << std::endl;
    }
  }

  void SayHelloStreamReply() {
    class ReactClientStream
        : public grpc::ClientReadReactor<helloworld::HelloReply> {
    public:
      ReactClientStream(helloworld::Greeter::Stub *stub) {
        m_request.set_name("world");
        stub->async()->SayHelloStreamReply(&m_context, &m_request, this);
        AddHold();
        StartCall();
        StartRead(&m_reply);
      }
      void OnReadDone(bool ok) override {
        if (!ok) {
          std::cout << "onReadDone Cancelled" << std::endl;
          RemoveHold();
          // 直接返回
          return;
        }
        std::cout << "Received reply: " << m_reply.message() << std::endl;
        StartRead(&m_reply);
      }
      void OnDone(const grpc::Status &s) override {
        std::cout << "Stream finished" << std::endl;
        m_status = std::move(s);
        std::unique_lock<std::mutex> lock(m_mu);
        m_done = true;
        m_cv.notify_all();
        delete this;
      }

      void AwaitForCompletion() {
        while (!m_done) {
          std::unique_lock<std::mutex> lock(m_mu);
          m_cv.wait(lock, [this] { return m_done; });
        }
      }

    private:
      helloworld::HelloReply m_reply;
      helloworld::HelloRequest m_request;
      grpc::ClientContext m_context;

      bool m_done{false};
      grpc::Status m_status;
      std::condition_variable m_cv;
      std::mutex m_mu;
    };
    ReactClientStream *reactor = new ReactClientStream(m_stub.get());
    reactor->AwaitForCompletion();
    // reactor 在流结束时会自动删除自己
    // 不需要手动删除 reactor
  }

  void SayHelloBindStream() {
    class ReactClientStream
        : public grpc::ClientBidiReactor<helloworld::HelloRequest,
                                         helloworld::HelloReply> {
    public:
      ReactClientStream(helloworld::Greeter::Stub *stub) {
        stub->async()->SayHelloBindStream(&m_context, this);
        AddHold();
        StartCall();
        m_request.set_name("world 20");
        StartWrite(&m_request);
      }
      void OnReadDone(bool ok) override {
        if (!ok) {
          std::cout << "onReadDone Cancelled" << std::endl;
          m_context.TryCancel();
          RemoveHold();
          return;
        }
        std::cout << "Received request: " << m_reply.message() << std::endl;
        StartRead(&m_reply);
      }
      void OnWriteDone(bool ok) override {
        if (!ok) {
          std::cout << "onWriteDone Cancelled" << std::endl;
          m_context.TryCancel();
          RemoveHold();
          return;
        }
        m_count++;
        if (m_count < 10) {
          std::cout << "send " << m_request.name() << std::endl;
          m_request.set_name("world " + std::to_string(m_count+20));
          StartWrite(&m_request);
        } else {
          std::cout << "onWriteDone Finish" << std::endl;
          // 结束流
          m_context.TryCancel();
          RemoveHold();
        }
      }
      void OnDone(const grpc::Status &s) override {
        std::cout << "Stream finished with status: " << s.error_message()
                  << std::endl;
        m_status = std::move(s);
        std::unique_lock<std::mutex> lock(m_mu);
        m_done = true;
        m_cv.notify_all();
        // reactor 在流结束时会自动删除自己
        delete this;
      }

      void AwaitForCompletion() {
        while (!m_done) {
          std::unique_lock<std::mutex> lock(m_mu);
          m_cv.wait(lock, [this] { return m_done; });
        }
      }

    private:
      helloworld::HelloRequest m_request;
      helloworld::HelloReply m_reply;
      grpc::ClientContext m_context;
      std::uint8_t m_count{0};

      bool m_done{false};
      grpc::Status m_status;
      std::condition_variable m_cv;
      std::mutex m_mu;
    };
    ReactClientStream *reactor = new ReactClientStream(m_stub.get());
    reactor->AwaitForCompletion();
  }

private:
  std::unique_ptr<helloworld::Greeter::Stub> m_stub;
};

int main(int argc, char **argv) {
  GreeterClient greeter(grpc::CreateChannel(
      "localhost:6666", grpc::InsecureChannelCredentials()));
  greeter.SayHello("world");
  greeter.SayHelloStream();
  greeter.SayHelloStreamReply();
  greeter.SayHelloBindStream();
  return 0;
}