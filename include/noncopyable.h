#ifndef NONCOPYABLE_H_
#define NONCOPYABLE_H_

/* 不可以通过拷贝构造和赋值运算符构造对象*/
class Noncopyable {
 public:
    Noncopyable(const Noncopyable&) = delete;
    void operator=(const Noncopyable&) = delete;

 protected:
    Noncopyable() = default;
    ~Noncopyable() = default;
};

#endif  // end NONCOPYABLE_H_
