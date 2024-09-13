#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>
#include "noncopyable.h"

class ThreadPool : public Noncopyable {
 public:
    explicit ThreadPool(int num);
    ~ThreadPool();

    void addTask(std::function<void()> task);

 private:
    std::vector<std::thread> _workers;
    std::queue<std::function<void()>> _task_queue;
    std::mutex _mtx;
    std::condition_variable _cond_var;
    std::atomic<bool> _stop;
};

#endif  // END THREADPOOL_H
