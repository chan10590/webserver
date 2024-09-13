#include "Buffer.h"
#include <iostream>

void Buffer::append(const char* str, int size) {
    if (str) {
        _buf.append(str, size);  // 二进制安全
    }
}

ssize_t Buffer::size() { return _buf.size(); }

std::string Buffer::toStr() const { return _buf; }

void Buffer::clear() { _buf.clear(); }

void Buffer::setBuf(const std::string& buf) { _buf = buf; }
