#ifndef WEB_UTIL_NONCOPYABLE_H_
#define WEB_UTIL_NONCOPYABLE_H_

namespace web {
/* 不可以通过拷贝构造和赋值运算符构造对象*/
class Noncopyable {
public:
    Noncopyable(const Noncopyable&) = delete;
    void operator=(const Noncopyable&) = delete;

protected:
    Noncopyable() = default;
    ~Noncopyable() = default;
};
}

#endif