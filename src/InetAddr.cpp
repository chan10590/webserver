#include "InetAddr.h"

namespace web{
InetAddr::InetAddr(const char* ip, uint16_t port): m_addr_len(sizeof(m_addr)){
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = inet_addr(ip);
    m_addr.sin_port = htons(port);
}
}