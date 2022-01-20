#pragma once

#include <mutex>
#include <queue>
#include <vector>
#include <thread>
#include <functional>

struct ThreadPool {
    size_t count;
    std::vector<std::thread> workers{};

    std::mutex lock{};
    std::queue<std::function<void()>> jobs{};

    explicit ThreadPool(size_t count = std::thread::hardware_concurrency()) : count(count) {
        workers.reserve(count);
    }

    ~ThreadPool() {
        wait();
    }

    void loop() {
        while (true) {
            lock.lock();
            if (jobs.empty()) {
                lock.unlock();
                break;
            }
            auto job = std::move(jobs.front());
            jobs.pop();
            lock.unlock();

            job();
        }
    }

    void start() {
        for (size_t i = 0; i < count; ++i) {
            workers.emplace_back(&ThreadPool::loop, this);
        }
    }

    void wait() {
        for (auto& worker : workers) {
            worker.join();
        }
        workers.clear();
    }
};
