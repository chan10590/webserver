#include "ThreadPool.h"
#include "Logger.h"

ThreadPool::ThreadPool(int thd_num) : _stop(false) {
    for (int i = 0; i < thd_num; ++i) {
        _workers.emplace_back([this]() {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(_mtx);
                    _cond_var.wait(lock, [this]() { return _stop || !_task_queue.empty(); });
                    if (_stop && _task_queue.empty()) return;
                    task = std::move(_task_queue.front());
                    _task_queue.pop();
                }
                LOG_INFO("sub loop");
                task();
            }
        });
    }
}
ThreadPool::~ThreadPool() {
    {
        std::lock_guard<std::mutex> lock(_mtx);
        _stop = true;
    }
    _cond_var.notify_all();
    for (std::thread& worker : _workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}
void ThreadPool::addTask(std::function<void()> task) {
    {
        std::lock_guard<std::mutex> lock(_mtx);
        _task_queue.push(std::move(task));
    }
    _cond_var.notify_one();
}
