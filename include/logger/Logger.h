#ifndef LOGGER_H_
#define LOGGER_H_

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include "noncopyable.h"

// 日志级别
enum LogLevel {
    INFO,   // 普通信息
    ERROR,  // 错误信息
    FATAL,  // core信息
    DEBUG   // 调试信息
};

// 设置成单例
class Logger : Noncopyable {
 public:
    static Logger& instance(const char* logpath = nullptr);  // 获取唯一实例
    ~Logger();

    void setLogLevel(LogLevel level);
    void writeLog(const std::string& msg, const char* file, int line);
    void start();  // 启动日志线程
    void stop();   // 停止日志线程

 private:
    Logger(const char* logpath);
    void logThreadFunc();  // 日志线程函数

    const char* m_logpath;
    LogLevel m_loglv;
    bool m_stopFlag;
    FILE* m_logFile;
    std::mutex m_mtx;
    std::queue<std::string> m_logQueue;
    std::condition_variable m_condVar;
    std::thread m_logThread;
};

#define LOG_BASE(level, LogmsgFormat, ...)                       \
    do {                                                         \
        Logger& log = Logger::instance();                        \
        log.setLogLevel(level);                                  \
        char buf[1024] = {0};                                    \
        snprintf(buf, sizeof(buf), LogmsgFormat, ##__VA_ARGS__); \
        log.writeLog(buf, __FILE__, __LINE__);                   \
    } while (0)

#define LOG_INFO(LogmsgFormat, ...) LOG_BASE(INFO, LogmsgFormat, ##__VA_ARGS__)
#define LOG_ERROR(LogmsgFormat, ...) LOG_BASE(ERROR, LogmsgFormat, ##__VA_ARGS__)
#define LOG_FATAL(LogmsgFormat, ...) LOG_BASE(FATAL, LogmsgFormat, ##__VA_ARGS__)
#define LOG_DEBUG(LogmsgFormat, ...) LOG_BASE(DEBUG, LogmsgFormat, ##__VA_ARGS__)

#endif  // END LOGGER_H_
