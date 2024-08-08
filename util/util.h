#ifndef WEB_UTIL_H_
#define WEB_UTIL_H_
namespace web{
    void errorExit(bool err_cond, const char* str);
    void setNonblocking(int fd);
}
#endif