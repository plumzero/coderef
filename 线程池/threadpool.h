
#include <stdint.h>
#include <stdarg.h>

#include <string>
#include <functional>
#include <queue>

#include <pthread.h>
#include <signal.h>

/**
 * 原则:
 * 1. 不能用到的不定义；
 * 2. 务必采用强类型写法；
 * 说明:
 * 1. ThreadCond 和 ThreadMutex 均不是构造时初始化，为了安全，这里禁用
 *    拷贝构造和赋值构造；
 * 2. 使用 std::function 运行时动态绑定函数，异常强；
 * 3. 虽说是 ThreadLog ，但却是非线程安全的；
 * 4. 名字空间不要与类名冲突；
 * 5. ThreadLog 中(为文件指针)定义了移动拷贝构造和移动赋值构造，但经测
 *    试未对原对象形成严格约束(原对象可能仍能使用指针读写)，虽如此但相
 *    比拷贝构造和赋值构造，其对内存管理良好；
 * 6. std::move 操作后对原内存的改变是未知的，可能置空，也可能依旧，但
 *    无论怎么，原内存已经无法再安全的使用了；
 */
/**
 * 对于多任务的处理方法:
 * 1. 线程池处理，更接近同步；
 * 2. 异步回调到处理队列中进行处理；
 */

/// 测试结构体 for task
typedef struct _test_task_struct {
    int t_i;
    double t_f;
    std::string t_s;
} TestTaskStruct;

namespace ThreadPoolNS
{
    typedef enum { 
        TP_OK     = 0, 
        TP_ERROR  = -1,
    } TpErrCode;
    
    typedef enum {
        TP_LOG_LEVEL_STDE     = 0,
        TP_LOG_LEVEL_EMER     = 1,
        TP_LOG_LEVEL_ALER     = 2,
        TP_LOG_LEVEL_CRIT     = 3,
        TP_LOG_LEVEL_ERRO     = 4,
        TP_LOG_LEVEL_WARN     = 5,
        TP_LOG_LEVEL_NOTI     = 6,
        TP_LOG_LEVEL_INFO     = 7,
        TP_LOG_LEVEL_DBUG     = 8,
    } TP_LOG_LEVEL;
    
    typedef int32_t   tp_int_t;
    typedef int64_t   tp_long_t;
    typedef uint32_t  tp_uint_t;
    typedef uint64_t  tp_ulong_t;
    
    typedef tp_int_t  tp_err_t;

    class CertainEvent
    {
    public:
        bool                        active;
        bool                        complete;
        std::function<void(void*)>  handler;
    public:
        CertainEvent()
            : active(false), complete(false) { handler = std::bind(certain_event_handler, std::placeholders::_1); }
        static void certain_event_handler(void *args);
    };
    
    class ThreadTask
    {
    public:
        tp_uint_t                           id;
        void                               *ctx;        /** task handle, used to bind the user data generally */
        std::function<void(ThreadTask*)>    handler;
        CertainEvent                        event;
    public:
        ThreadTask()
            : id(0) { handler = std::bind(&ThreadTask::thread_task_handler, std::placeholders::_1); }
        static void thread_task_handler(ThreadTask *task);
    };
    
    class ThreadMutex;
    
    class ThreadCond
    {
    public:
        pthread_cond_t cond;
    public:
        ThreadCond() {}
        ~ThreadCond() {}
        explicit ThreadCond(pthread_cond_t cond) { this->cond = cond; }
        ThreadCond(const ThreadCond &) = delete;
        ThreadCond &operator=(const ThreadCond &) = delete;
        inline operator pthread_cond_t*() { return &cond; }
        
        TpErrCode thread_cond_init();
        TpErrCode thread_cond_destroy();
        TpErrCode thread_cond_signal();
        TpErrCode thread_cond_wait(const ThreadMutex &m_ctx_r);
        
    };
    
    class ThreadMutex
    {
    public:
        pthread_mutex_t mtx;
    public:
        ThreadMutex() {}
        ~ThreadMutex() {}
        explicit ThreadMutex(pthread_mutex_t mtx) { this->mtx = mtx; }
        ThreadMutex(const ThreadMutex &) = delete;
        ThreadMutex &operator=(const ThreadMutex &) = delete;
        inline operator pthread_mutex_t*() { return &mtx; }
        
        TpErrCode thread_mutex_init();
        TpErrCode thread_mutex_destroy();
        TpErrCode thread_mutex_lock();
        TpErrCode thread_mutex_unlock();
    };
    
    class ThreadSpinLock
    {
    public:
        pthread_spinlock_t spinlock;
    public:
        ThreadSpinLock() {}
        ~ThreadSpinLock() {}
        explicit ThreadSpinLock(pthread_spinlock_t spinlock) { this->spinlock = spinlock; }
        ThreadSpinLock(const ThreadSpinLock &) = delete;
        ThreadSpinLock &operator=(const ThreadSpinLock &) = delete;
        inline operator pthread_spinlock_t*() { return &spinlock; }
        
        TpErrCode thread_spin_init();
        TpErrCode thread_spin_destroy();
        TpErrCode thread_spin_lock();
        TpErrCode thread_spin_unlock();
    };
    
    class ThreadLog
    {
    public:
        TP_LOG_LEVEL        level;
        FILE                *fp;
    public:
        ThreadLog() { this->fp = stdout; level = TP_LOG_LEVEL_DBUG; }
        ~ThreadLog() {}
        explicit ThreadLog(FILE *fp) { this->fp = fp; level = TP_LOG_LEVEL_INFO; }
        ThreadLog(const ThreadLog &) = delete;
        ThreadLog &operator=(const ThreadLog &) = delete;
        ThreadLog(ThreadLog &&tlog) 
            : fp(std::move(tlog.fp)), level(std::move(tlog.level)) {}
        ThreadLog &operator=(ThreadLog &&tlog)
        {
            fp = std::move(tlog.fp); 
            level = std::move(tlog.level);
        }
                
        void thread_log_print(TP_LOG_LEVEL lvl, const char *fmt, ...);
        void thread_log_vprintf(TP_LOG_LEVEL lvl, const char *fmt, va_list ap);
    };
    
    class ThreadPool
    {
    #define TPECHO_IN(cp, level, fmt, ...)                          \
        do {                                                        \
            cp->m_log.thread_log_print(level, fmt, __VA_ARGS__);    \
        } while (0)

    #define TPECHO(level, fmt, ...)                                 \
        do {                                                        \
            TPECHO_IN(this, ThreadPoolNS::TP_LOG_LEVEL_ ## level,   \
                        "%s %2d " fmt "\n", "[" #level "] ",        \
                        __LINE__, ##__VA_ARGS__);                   \
        } while (0)
    #define S_TPECHO(cp, level, fmt, ...)                           \
        do {                                                        \
            TPECHO_IN(cp, ThreadPoolNS::TP_LOG_LEVEL_ ## level,     \
                        "%s %2d " fmt "\n", "[" #level "] ",        \
                        __LINE__, ##__VA_ARGS__);                   \
        } while (0)
    public:
        ThreadMutex                 m_mtx;
        ThreadCond                  m_cond;
        ThreadSpinLock              m_spinlock;
        ThreadLog                   m_log;
        std::queue<ThreadTask>      m_queues;
        std::queue<ThreadTask>      m_queues_done;
        tp_int_t                    m_waiting;
        std::string                 m_name;
        tp_uint_t                   m_threads;
        tp_int_t                    m_maxqueue;
        static tp_uint_t            m_s_task_id;
    public:         // function handler
        std::function<void(CertainEvent*)>  m_handler_event;
    private:
        TpErrCode thread_pool_queue_init(std::queue<ThreadTask> *queue);
        static void *thread_pool_cycle(void *userp);
        static void thread_pool_exit_handler(void *userp);
        static void thread_pool_event_handler(void *userp);
    public:
        ThreadPool() 
            : m_waiting(0), m_name("RESONATOR"), m_threads(8), m_maxqueue(256) {}
        ~ThreadPool() {}
        TpErrCode thread_pool_init(ThreadLog &tlog);
        TpErrCode thread_pool_destroy();
        TpErrCode thread_pool_task_post(ThreadTask &task);     /** 注册任务 */
    };  
};

