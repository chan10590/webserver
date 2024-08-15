#include "ThreadPool.h"
#include <stdexcept>

namespace web {
ThreadPool::ThreadPool(int thd_num) : m_stop(false) {
    for (int i = 0; i < thd_num; ++i) {
        m_threadpool.emplace_back(std::thread([this]() {
            while (1) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(m_task_mutex);
                    m_task_cv.wait(lock, [this]() { return m_stop || !m_task_queue.empty(); });
                    if (m_stop && m_task_queue.empty()) return;
                    task = m_task_queue.front();
                    m_task_queue.pop();
                }
                task();
            }
        }));
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(m_task_mutex);
        m_stop = true;
    }
    m_task_cv.notify_all();
    for (std::thread& th : m_threadpool) {
        if (th.joinable()) {
            th.join();
        }
    }
}

}  // namespace web
