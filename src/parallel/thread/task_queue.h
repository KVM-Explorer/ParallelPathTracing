#pragma once
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <future>

class TaskQueue {
  private:
    int maxConCurency = 10;
    std::atomic<bool> stop = false;
    std::condition_variable condition;
    std::mutex mutex;
    std::vector<std::thread> threads;
    std::queue<std::packaged_task<void()>> tasks;

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
                    std::packaged_task<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->mutex);
                        this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });
                        if (this->stop && this->tasks.empty()) {
                            return;
                        }
                        task = std::move(this->tasks.front());
                     
                        this->tasks.pop();          // 没有考虑到读取过程中,任务队列添加元素导致扩容似的内存失效
                    }
                    task();
                }
            }));
        }
    }

    void add(std::function<void()> func) {
        std::unique_lock<std::mutex> lock(mutex);
        tasks.emplace(std::move(func));
    }

    void joinAll() {

        stop.store(true);
        condition.notify_all();         // 记得重新唤醒,因为条件变量检查过后进入休眠
        for (int i = 0; i < threads.size(); i++) {
            threads[i].join();
        }
    }
};