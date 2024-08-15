#ifndef SRC_INC_THREADPOOL_H_
#define SRC_INC_THREADPOOL_H_

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <utility>
#include <vector>

namespace web {

class ThreadPool {
 private:
    std::vector<std::thread> m_threadpool;
    std::queue<std::function<void()>> m_task_queue;
    std::mutex m_task_mutex;
    std::condition_variable m_task_cv;
    bool m_stop;

 public:
    explicit ThreadPool(int thd_num = 20);
    ~ThreadPool();

    template <class F>
    auto addTask2Queue(F&& f) -> std::future<typename std::invoke_result<std::decay_t<F>>::type>;
};

template <class F>
auto ThreadPool::addTask2Queue(F&& f)
    -> std::future<typename std::invoke_result<std::decay_t<F>>::type> {
    using return_type = typename std::invoke_result<std::decay_t<F>>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(std::forward<F>(f));
    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(m_task_mutex);
        if (m_stop) {
            throw std::runtime_error("ThreadPool stop! No add task!");
        }
        m_task_queue.emplace([task]() { (*task)(); });
    }
    m_task_cv.notify_one();
    return res;
}

}  // namespace web

#endif  // SRC_INC_THREADPOOL_H_
