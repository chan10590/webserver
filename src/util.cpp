#include "util.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

namespace web {
/* 打印错误，并退出程序
 * @param[in] err_cond 错误条件 bool
 * @param[in] str 错误输出提示 const char*
 */
void errorExit(bool err_cond, const char* str) {
    if (err_cond) {
        perror(str);
        exit(1);
    }
}

}  // namespace web
