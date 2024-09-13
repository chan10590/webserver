#include "Logger.h"

int main() {
    Logger::instance().start();
    LOG_INFO("This is an info log.");
    LOG_ERROR("This is an error log.");
    LOG_FATAL("This is a fatal log.");
    LOG_DEBUG("This is a debug log.");
    int a = 0;
    LOG_INFO("input a: %d", a);
    int b = 1;
    LOG_INFO("input b: %d", b);
    Logger::instance().stop();
    return 0;
}
