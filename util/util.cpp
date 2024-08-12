#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

namespace web {
    /* 打印错误，并退出程序
    * @param[in] err_cond 错误条件 bool
    * @param[in] str 错误输出提示 const char*
    */
    void errorExit(bool err_cond, const char* str) {
        if(err_cond){
            perror(str);
            exit(1);
        }
    }

    /*设置非阻塞*/
    void setNonblocking(int fd){
        int flag = fcntl(fd, F_GETFL, 0);
        flag |= O_NONBLOCK;
        errorExit(fcntl(fd, F_SETFL, flag) == -1, "nonblock fail!");
    }
    
}
