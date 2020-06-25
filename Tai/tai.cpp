
#include "tai.h"

namespace TC
{
Tai::Tai()
{
    struct timeval tus;
    gettimeofday(&tus, nullptr);
    m_time = (uint64_t)tus.tv_sec - 1072915200;
}

Tai::Tai(const std::string& strTime)
{
    int ret = -1;
    int year, month, day, hour, minute, second;
    struct tm ts;
    time_t epoch;
    
    year = month = day = hour = minute = second = 0;
    
    ret = sscanf(strTime.c_str(), "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
    if (ret != 6) {
        throw;
    }
    
    ts.tm_year = year - 1900;
    ts.tm_mon  = month - 1;
    ts.tm_mday = day;
    ts.tm_hour = hour;
    ts.tm_min  = minute;
    ts.tm_sec  = second;
    
    epoch = mktime(&ts);
    m_time = (uint64_t)epoch - 1072915200;
}


Tai& Tai::addYears(uint64_t y)
{
    struct tm* st;
    time_t epochTime = m_time + 1072915200;
    
    st = localtime(&epochTime);
            
    uint64_t i, days, month, year;
    bool first = true;
    month = st->tm_mon + 1;
    days = 0;
    for (i = 0; i < y; i++) {
        days += 365;
        year = st->tm_year + 1900 + i;
        if ((year % 4 == 0 && year % 100 != 0) || 
            (year % 400 == 0 && year % 3200 != 0) ||
            year % 172800 == 0) {
            if (first) {
                if (month <= 2) {
                    days += 1;
                }
                first = false;
            } else {
                days += 1;
            }
        }
    }
    
    m_time += days * 24 * 3600;
    
    return *this;
}

Tai& Tai::addMonths(uint64_t m)
{
    struct tm* st;
    time_t epochTime = m_time + 1072915200;
    
    st = localtime(&epochTime);
        
    uint64_t i, days, month, year;
    year = st->tm_year + 1900;
    days = 0;
    for (i = 0; i < m; i++) {
        month = (st->tm_mon + 1 + i) % 12;
        if (month % 12 == 0) {
            year += 1;
            month = 1;
        }
        if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12) {
            days += 31;
        } else if (month == 4 || month == 6 || month == 9 || month == 11) {
            days += 30;
        } else if (month == 2) {
            if((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0 && year % 3200 != 0) || year % 172800 == 0) {
                days += 29;
            } else {
                days += 28;
            }
        }
    }
    
    m_time += days * 24 * 3600;
    
    return *this;
}

Tai& Tai::addWeeks(uint64_t w)
{
    m_time += Week(w);
    return *this;
}
Tai& Tai::addDays(uint64_t d)
{
    m_time += Day(d);
    return *this;
}
Tai& Tai::addHours(uint64_t h)
{
    m_time += Hour(h);
    return *this;
}
Tai& Tai::addMinutes(uint64_t m)
{
    m_time += Minute(m);
    return *this;
}
Tai& Tai::addSeconds(uint64_t s)
{
    m_time += Second(s);
    return *this;
}

void Tai::printTime()
{
    char tmbuf[64] = { 0 };
    time_t epoch_time = m_time + 1072915200;
    struct tm *st = localtime(&epoch_time);
    strftime(tmbuf, sizeof(tmbuf), "%Y-%m-%d %H:%M:%S", st);
    
    std::cout << tmbuf << std::endl;
}

};

