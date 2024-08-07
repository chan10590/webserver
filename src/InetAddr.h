#ifndef WEB_INETADDR_H_
#define WEB_INETADDR_H_
#include <arpa/inet.h>
#include <string.h>

namespace web{
/*存放地址相关信息
 * addr      存放IP地址及端口信息
 * addr_len  addr结构体大小
 */
class InetAddr
{
public:
    // 在Socket类中会被使用，因此需要设为public；设为private就需要成员函数获取私有的成员变量
    struct sockaddr_in m_addr;
    socklen_t m_addr_len;
    InetAddr():m_addr_len(sizeof(m_addr)){
        memset(&m_addr, 0, sizeof(m_addr));
    }
    InetAddr(const char* ip, uint16_t port);
    ~InetAddr(){}
};
}

#endif