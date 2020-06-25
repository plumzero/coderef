
#ifndef __TAI_time_H__
#define __TAI_time_H__

#include <cstdio>
#include <cstdint>
#include <string>
#include <iostream>
#include <sys/time.h>

/** 
 * 准确性:
 *  年 - ok
 *  月 - ok
 *  周 - ok
 *  日 - ok
 *  时 - ok
 *  分 - ok
 *  秒 - ok
 */

namespace TC
{
class Tai
{
public:
    Tai();
    /** 格式: "2019-11-15 18:22:08" */
    /** 不对日期格式进行校验，仅用于测试 */
    Tai(const std::string& strTime);
    ~Tai(){}
    
    Tai& addYears(uint64_t y);
    Tai& addMonths(uint64_t m);
    Tai& addWeeks(uint64_t w);
    Tai& addDays(uint64_t d);
    Tai& addHours(uint64_t h);
    Tai& addMinutes(uint64_t m);
    Tai& addSeconds(uint64_t s);
    
    operator uint64_t() { return m_time; }
    
    void printTime();
    
private:
  uint64_t  m_time;
};

class Week
{
public:
    Week() = delete;
    ~Week(){}
    explicit Week(uint64_t w) { W = w; }
    operator uint64_t(){ return W * 7 * 24 * 3600; }
private:
    uint64_t W;
};

class Day
{
public:
    Day() = delete;
    ~Day(){}
    explicit Day(uint64_t d) { D = d; }
    operator uint64_t(){ return D * 24 * 3600; }
private:
    uint64_t D;
};

class Hour
{
public:
    Hour() = delete;
    ~Hour(){}
    explicit Hour(uint64_t h) { H = h; }
    operator uint64_t(){ return H * 3600; }
private:
    uint64_t H;
};

class Minute
{
public:
    Minute() = delete;
    ~Minute(){}
    explicit Minute(uint64_t m) { M = m; }
    operator uint64_t(){ return M * 60; }
private:
    uint64_t M;
};

class Second
{
public:
    Second() = delete;
    ~Second(){}
    explicit Second(uint64_t s) { S = s; }
    operator uint64_t(){ return S; }
private:
    uint64_t S;
};

};
#endif /* __TAI_time_H__ */
