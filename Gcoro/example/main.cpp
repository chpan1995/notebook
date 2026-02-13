#include "Gcorotask.h"
#include "Glazytask.h"
#include "Gcorogenerator.h"
#include "Gcoroasyncgenerator.h"
#include "WaitFor.hpp"

#include <QCoreApplication>
#include <iostream>
#include <thread>
#include <exception>

GCoro::Task<void> testCoro() {
    co_return;
}

// Simple generator that produces `count` values from 0 to `(count-1)`.
GCoro::Generator<int> iota(int count) {
    for (int i = 0; i < count; ++i) {
        // Yields a value and suspends the generator.
        co_yield i;
    }
}

void counter() {
    // Creates a new initially suspended generator coroutine
    auto generator = iota(10);
    // Resumes the coroutine, obtains first value and returns
    // an iterator representing the first value.
    auto it = generator.begin();
    // Obtains a past-the-end iterator.
    const auto end = generator.end();

    // Loops until the generator finishes.
    while (it != end) {
        // Reads the current value from the iterator.
        std::cout << *it << std::endl;
        // Resumes the generator until it co_yields another value.
        ++it;
    }

    // The code above can be written more consisely using ranged based for-loop
    for (const auto value : iota(10)) {
        std::cout << value << std::endl;
    }
}


GCoro::AsyncGenerator<int> generatorThatThrows() {
    while (true) {
        const int val = 1;
        if (val == 0) {
            throw std::runtime_error("Division by zero!");
        }
        co_yield 42 / val;
    }
}

GCoro::Task<std::vector<int>> divide42ForNoReason(std::size_t count) {
    auto generator = generatorThatThrows();
    std::vector<int> numbers;
    try {
        // Might throw if generator generates 0 immediately.
        auto it = co_await generator.begin();
        while (numbers.size() < count) {
            numbers.push_back(*it);
            co_await ++it; // might throw
        }
        std::cout << "Generated " << numbers.size() << " numbers." << std::endl;
        co_return numbers;
    } catch (const std::runtime_error &e) {
        // We were unable to generate all numbers
    }
}

int main(int argc,char** argv) {
    QCoreApplication a(argc,argv);

    std::uint32_t num = 0x43FA9110;
    float f = std::bit_cast<float>(num);
    std::cout << "Bit pattern: 0x" << std::hex << num << std::dec << ", as float: " << f << std::endl;
    auto task = testCoro().then([]() {
        std::cout << "Coroutine completed!" << std::endl;
    }, [](const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    });
    if (task.isReady()) {
        std::cout << "Coroutine is ready!" << std::endl;
    } else {
        std::cout << "Coroutine is not ready yet." << std::endl;
    }
    counter();
    // 为什么不从返回值拿值，因为协程可能没执行完，所以在协程内部拿值更合理
    auto task2 = divide42ForNoReason(3);

    GCoro::connect(std::move(task2), QCoreApplication::instance(), [](auto &&result) {
        // std::cout << result << std::endl;
    });
    // 这里子的eventloop里面等待协程结束拿值
    auto res = GCoro::waitFor(divide42ForNoReason(3));

    std::cout << "res size " << res.size() << std::endl;
    return a.exec();
}