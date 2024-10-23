#ifndef TIMER_H
#define TIMER_H

#include <string>
#include <chrono>

class Timer {
public:
    // 构造函数：记录开始时间
    explicit Timer(const std::string& name );

    // 析构函数：记录结束时间并计算持续时间
    ~Timer();

    // // 重置计时器
    // void reset();

    // // 获取当前持续时间
    // double elapsed() const;

private:
    std::string name_;  // Timer 的名称（可选）
    std::chrono::time_point<std::chrono::high_resolution_clock> start_;  // 开始时间
};

#endif // TIMER_H
