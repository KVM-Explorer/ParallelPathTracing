#pragma once
#include <functional>
#include <queue>
#include <thread>

class TaskQueue {
  public:
    TaskQueue(int max_concurency) : maxConCurency(max_concurency) {}
    TaskQueue() {
        int size = std::thread::hardware_concurrency();
        maxConCurency = size != 0 ? size : 1;
    }

    void add(std::function<void()> func) {
        if (queue.size() > maxConCurency) {
            queue.front().join();
            queue.pop();
        }
        queue.emplace([f = func]() {
            f();
        });
    }

    void joinAll() {
        while (!queue.empty()) {
            if (queue.front().joinable())
                queue.front().join();
            queue.pop();
        }
    }

  private:
    int maxConCurency = 10;
    std::queue<std::thread> queue;
};