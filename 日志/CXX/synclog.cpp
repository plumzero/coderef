
#include <stdio.h>
#include <stdint.h>
#include <string>
#include <queue>
#include <map>
#include <utility>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <type_traits>
#include <sys/time.h>
#include <unistd.h>

/// 同步日志对比

typedef enum { STDE, EMER, ALER, CRIT, ERRO, WARN, NOTI, INFO, DBUG } Level;

static std::map<Level, std::string> m_descr;
std::ofstream m_ofs;
Level m_level = INFO;

void log_decorate(const std::pair<std::thread::id, std::pair<Level, std::string> >& log_pair)
{
    if (log_pair.second.first > m_level)
        return;

    char buf[64] = { 0 };
    time_t now;
    int n;
    std::ostringstream oss;
    
    oss << log_pair.first;
    
    time(&now);
    tm *tm_now = localtime(&now);
    n = snprintf(buf, 64, "#%llu %04d.%02d.%02d %02d:%02d:%02d",
                        std::stoull(oss.str()),
                        tm_now->tm_year + 1900,
                        tm_now->tm_mon + 1,
                        tm_now->tm_mday,
                        tm_now->tm_hour,
                        tm_now->tm_min,
                        tm_now->tm_sec);

    m_ofs << std::string(buf, n) << m_descr[log_pair.second.first] << log_pair.second.second << std::endl;  // std::flush
    
    return;
}

void log_descr()
{
    m_descr[STDE] = " [STDE] ";
    m_descr[EMER] = " [EMER] ";
    m_descr[ALER] = " [ALER] ";
    m_descr[CRIT] = " [CRIT] ";
    m_descr[ERRO] = " [ERRO] ";
    m_descr[WARN] = " [WARN] ";
    m_descr[NOTI] = " [NOTI] ";
    m_descr[INFO] = " [INFO] ";
    m_descr[DBUG] = " [DBUG] ";
}

int main()
{
    int total = 400000, i;
        
    m_ofs.open("sync.log", std::ofstream::out | std::ofstream::app);
    if (! m_ofs.is_open()) {
        printf("not open\n");
        return -1;
    }
    char buf[1024];
    int n;
    std::pair<std::thread::id, std::pair<Level, std::string> > log_pair;
    
    for (i = 0; i < total; i++) {
        n = snprintf(buf, 1024, " %03d ======> hello world, this thread", i);
        log_pair = std::make_pair(std::this_thread::get_id(), std::make_pair(INFO, std::string(buf, n)));
        log_decorate(log_pair);
        usleep(100);        // 模拟工作时间
    }
    
    m_ofs.close();

    return 0;
}