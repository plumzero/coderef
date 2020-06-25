
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/time.h> 

/**
 * 简单的定时器
 * 注意：微秒级计时不太精确
 *
 * 多核时间不宜再用 x86 的 RDTSC 指令测试指令周期和时间，原因如下:
 * 1. 不能保证同一块主板上每个核的 TSC 是同步的；
 *    每个核心拥有一个 RDTSC，不能保证每个核心在同一时刻启动 RDTSC 
 *    计时，又由于 cpu 抢占机制，所读取的当前 RDTSC 未必时原有的
 *    RDTSC。
 * 2. CPU 的时钟频率可能变化，例如笔记本的节能模式；
 * 3. 乱序执行导致 RDTSC 测得的周期数不准；
 *
 * RDTSC 适用于应用绑定到核心的情况，即一个核心负责一个应用，应用不
 * 会使用到两个核心。
 *
 * 通常使用系统调用 gettimeofday 实现计时，如果需要更高精度的计时，
 * 可以通过 clock_gettime 函数，以 CLOCK_MONOTONIC 参数调用。
 */

/**
 * C++ 相关
 * 常成员函数内部调用本类的其他成员函数时，后者必须也为常成员函数。
 */ 

class Timer
{
public:
    Timer() : m_ticks(0) {}
    void start() { m_ticks = getCurrentTicks(); }
    
    uint64_t getElapsedMicroseconds() const {
        return static_cast<uint64_t>(getCurrentTicks() - m_ticks);
    }
    
    uint64_t getElapsedMilliseconds() const {
        return static_cast<uint64_t>(getElapsedMicroseconds() / 1000);
    }
    
    double getElapsedSeconds() const { return getElapsedMicroseconds() / 1000000.0; }

private:
    uint64_t getCurrentTicks() const {
        timeval t;
        gettimeofday(&t, 0);
        return static_cast<uint64_t>(t.tv_sec) * 1000000 + static_cast<uint64_t>(t.tv_usec);
    }
    
private:
    uint64_t m_ticks;
};

int main()
{
    Timer timer;
    timer.start();
    
    usleep(150000);
    
    printf("sleep for %lu milliseconds\n", timer.getElapsedMilliseconds());
    
    return 0;
}
