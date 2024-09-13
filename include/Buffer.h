#ifndef BUFFER_H_
#define BUFFER_H_

#include <string>

class Buffer {
 public:
    void append(const char* str, int size);
    ssize_t size();
    std::string toStr() const;
    void clear();
    void setBuf(const std::string& buf);

 private:
    std::string _buf;
};

#endif  // end BUFFER_H_
