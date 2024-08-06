#include "util.h"
#include <fcntl.h>
namespace web{
    /* 设置套接字为非阻塞模式
     * @param fd 套接字 int
    */
    void setNonBlock(int fd) {
        int flag = fcntl(fd, F_GETFL, 0);
        flag |= O_NONBLOCK;
        errorExit(fcntl(fd, F_SETFL, flag) < 0, "nonblock fail!");
}
}   