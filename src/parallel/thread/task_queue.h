#pragma once
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class TaskQueue {
  private:
    int maxConCurency = 10;
    std::atomic<bool> stop = false;
    std::condition_variable condition;
    std::mutex mutex;
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;

  public:
    TaskQueue() {
        int size = std::thread::hardware_concurrency();
        maxConCurency = size != 0 ? size : 1;
        TaskQueue(maxConCurency);
    }
    TaskQueue(int max_concurency) : maxConCurency(max_concurency) {
        for (int i = 0; i < maxConCurency; i++) {
            threads.push_back(std::thread([this]() {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->mutex);
                        this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });
                        if (this->stop && this->tasks.empty()) {
                            return;
                        }
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }
                    task();
                }
            }));
        }
    }

    void add(std::function<void()> func) {
        tasks.emplace(func);
    }

    void joinAll() {

        stop.store(true);

        for (int i = 0; i < threads.size(); i++) {
            threads[i].join();
        }
    }
};