#include "Logger.h"
// #include <chrono>
#include <ctime>
#include <iostream>
#include <stdexcept>

Logger::Logger(const char* logpath) : m_logpath(logpath), m_loglv(INFO), m_stopFlag(false), m_logFile(nullptr) {
    if (!m_logpath) {
        m_logpath = "../log/default.log";
    }
    m_logFile = fopen(m_logpath, "a");
    if (!m_logFile) {
        throw std::runtime_error("Unable to open log file");
    }
}

Logger::~Logger() {
    stop();
    if (m_logFile) {
        fclose(m_logFile);
    }
}

Logger& Logger::instance(const char* logpath) {
    static Logger* logger = nullptr;
    static std::mutex initMutex;

    if (logger == nullptr) {
        std::lock_guard<std::mutex> lock(initMutex);
        if (logger == nullptr) {
            logger = new Logger(logpath);
            logger->start();
        }
    }
    return *logger;
}

void Logger::setLogLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(m_mtx);
    m_loglv = level;
}

void Logger::writeLog(const std::string& msg, const char* file, int line) {
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        std::string logMsg = std::string(file) + ":" + std::to_string(line) + ": " + msg;
        m_logQueue.push(logMsg);
    }
    m_condVar.notify_one();
}

void Logger::start() {
    std::lock_guard<std::mutex> lock(m_mtx);
    if (!m_logThread.joinable()) {
        m_logThread = std::thread(&Logger::logThreadFunc, this);
    }
}

void Logger::stop() {
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        m_stopFlag = true;
    }
    m_condVar.notify_all();
    if (m_logThread.joinable()) {
        m_logThread.join();
    }
}

void Logger::logThreadFunc() {
    while (true) {
        std::string msg;
        {
            std::unique_lock<std::mutex> lock(m_mtx);
            m_condVar.wait(lock, [this] { return !m_logQueue.empty() || m_stopFlag; });
            if (m_stopFlag && m_logQueue.empty()) {
                break;
            }
            msg = m_logQueue.front();
            m_logQueue.pop();
        }

        // auto now = std::chrono::system_clock::now();
        // auto now_c = std::chrono::system_clock::to_time_t(now);
        std::time_t now = std::time(nullptr);
        char timeStr[100];
        std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

        std::string logLevelStr;
        switch (m_loglv) {
            case INFO:
                logLevelStr = "[INFO]";
                break;
            case ERROR:
                logLevelStr = "[ERROR]";
                break;
            case FATAL:
                logLevelStr = "[FATAL]";
                break;
            case DEBUG:
                logLevelStr = "[DEBUG]";
                break;
            default:
                break;
        }

        if (m_logFile) {
            // fprintf(m_logFile, "%s %s %s\n", std::ctime(&now_c), logLevelStr.c_str(), msg.c_str());
            fprintf(m_logFile, "%s %s %s\n", timeStr, logLevelStr.c_str(), msg.c_str());
            fflush(m_logFile);
        }
    }
}
