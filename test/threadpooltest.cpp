#include <iostream>
#include <mutex>
#include <string>
#include "../src/inc/ThreadPool.h"

std::mutex print_mutex;

void addTaskExp(web::ThreadPool* pool, std::function<void()> task) {
    try {
        pool->addTask2Queue(task);
    } catch (const std::runtime_error& e) {
        std::cerr << __FILE__ << " " << __LINE__ << " line : Catch a runtime error: " << e.what()
                  << std::endl;
    }
}

int main(int argc, char const* argv[]) {
    int i = 0;
    web::ThreadPool* pool = new web::ThreadPool();
    for (i = 0; i < 5; ++i) {
        addTaskExp(pool, [i]() {
            // 加锁可保证打印的原子性，但是另一个线程必须等上一个线程打印完才能打印，影响并发性能
            std::lock_guard<std::mutex> guard(print_mutex);
            std::cout << "hello, " << i << std::endl;
        });
    }
    for (i = 0; i < 5; ++i) {
        addTaskExp(pool, [i]() {
            std::lock_guard<std::mutex> guard(print_mutex);
            std::cout << "world, " << i << std::endl;
        });
    }
    delete pool;
    return 0;
}
