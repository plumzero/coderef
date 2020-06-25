
#include <stdio.h>
#include <stdint.h>
#include <time.h>
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

/**
 * 为了构造安全，对各种构造函数进行限制
 */
class AsyncLog
{
public:
    typedef enum { STDE, EMER, ALER, CRIT, ERRO, WARN, NOTI, INFO, DBUG } Level;
    typedef enum { OK, ERR_STREAM } ECode;

public:
    std::mutex                      m_mtx;
    std::condition_variable         m_cond;

    std::ofstream                   m_ofs;
    std::queue<std::pair<std::thread::id, std::pair<Level, std::string> > > m_queues;
    Level   m_level;
    static std::map<Level, std::string> m_descr;
    
public:
    explicit AsyncLog(const std::string logname) 
        : m_level(INFO)
    { 
        m_ofs.open(logname.c_str(), std::ofstream::out | std::ofstream::app);   
    }
    AsyncLog() = delete;
    AsyncLog(const AsyncLog& al) = delete;
    AsyncLog& operator=(const AsyncLog& al) = delete;
    AsyncLog(const AsyncLog&& al) = delete;
    AsyncLog& operator=(const AsyncLog&& al) = delete;
    ~AsyncLog() { if(m_ofs.is_open()) m_ofs.close(); }
    
    AsyncLog& operator<<(std::pair<std::thread::id, std::pair<Level, std::string> > &log_pair);
    void log_decorate(const std::pair<std::thread::id, std::pair<Level, std::string> >& log_pair);
    void log_run();
    
    static void log_descr();
};
