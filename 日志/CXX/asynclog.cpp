
#include "asynclog.h"

#if ( __i386__ || __i386 || __amd64__ || __amd64 || __x86__ )
#define likely(x)            __builtin_expect(!!(x), 1)
#define unlikely(x)          __builtin_expect(!!(x), 0)
#else
#define likely(x)
#define unlikely(x)
#endif

#if ( __i386__ || __i386 || __amd64__ || __amd64 )
#define tp_cpu_pause()          __asm__ ("pause")
#elif ( __x86__ )
#define ngx_cpu_pause()         __asm__ (".byte 0xf3, 0x90")
#else
#define tp_cpu_pause()
#endif

#if ( __i386__ || __i386 || __amd64__ || __amd64 || __x86__ )
#define tp_memory_barrier()     __asm__ volatile ("" ::: "memory")
#else
#define tp_memory_barrier()
#endif

std::map<AsyncLog::Level, std::string> AsyncLog::m_descr = std::map<AsyncLog::Level, std::string>();

void AsyncLog::log_descr()
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

void AsyncLog::log_decorate(const std::pair<std::thread::id, std::pair<AsyncLog::Level, std::string> >& log_pair)
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

AsyncLog& AsyncLog::operator<<(std::pair<std::thread::id, std::pair<AsyncLog::Level, std::string> > &log_pair)
{
    std::unique_lock<std::mutex> guard(m_mtx);
    m_queues.push(std::move(log_pair));
    m_cond.notify_one();

    return *this;
}

void AsyncLog::log_run()
{   
    if (! m_ofs.is_open()) {
        return ;
    }
    
    for (;;) {
        std::unique_lock<std::mutex> guard(m_mtx);
        if (! m_cond.wait_for(guard, std::chrono::seconds(1), [this]{ return ! m_queues.empty(); })) {
            continue;
        }
        auto log_pair = std::move(m_queues.front());
        m_queues.pop();
        guard.unlock();
        
        log_decorate(log_pair);
    }
}