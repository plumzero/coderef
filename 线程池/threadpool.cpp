
#include "threadpool.h"
#include <string.h>
#include <time.h>
#include <sys/time.h>

#define TEST 1

#ifdef TEST

#include <stdio.h>

#endif



namespace ThreadPoolNS
{

#if ( __i386__ || __i386 || __amd64__ || __amd64 || __x86__ )
#define tp_memory_barrier()     __asm__ volatile ("" ::: "memory")
#else
#define tp_memory_barrier()
#endif

    /** Thread Condition Signal */
    TpErrCode ThreadCond::thread_cond_init()
    {
        tp_err_t err;
        
        err = pthread_cond_init(&cond, nullptr);
        
        return err == 0 ? TP_OK : TP_ERROR;
    }
    TpErrCode ThreadCond::thread_cond_destroy()
    {
        tp_err_t err;
        
        err = pthread_cond_destroy(&cond);
        
        return err == 0 ? TP_OK : TP_ERROR;
    }
    TpErrCode ThreadCond::thread_cond_signal()
    {
        tp_err_t err;
        
        err = pthread_cond_signal(&cond);
        
        return err == 0 ? TP_OK : TP_ERROR;
    }   
    TpErrCode ThreadCond::thread_cond_wait(const ThreadMutex &m_ctx_r)
    {
        tp_err_t err;
                
        err = pthread_cond_wait(&cond, (pthread_mutex_t *)const_cast<ThreadMutex &>(m_ctx_r));
        
        return err == 0 ? TP_OK : TP_ERROR;
    }
    /** Thread Mutex */
    TpErrCode ThreadMutex::thread_mutex_init()
    {
        tp_err_t err;
        pthread_mutexattr_t  attr;
                
        err = pthread_mutexattr_init(&attr);
        if (err) return TP_ERROR;
                
        err = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
        if (err) return TP_ERROR;
                
        err = pthread_mutex_init(&mtx, &attr);
        if (err) return TP_ERROR;
                
        err = pthread_mutexattr_destroy(&attr);
        return err == 0 ? TP_OK : TP_ERROR;
    }
    
    TpErrCode ThreadMutex::thread_mutex_destroy()
    {
        tp_err_t err;
        
        err = pthread_mutex_destroy(&mtx);
        
        return err == 0 ? TP_OK : TP_ERROR;
    }
    TpErrCode ThreadMutex::thread_mutex_lock()
    {
        tp_err_t err;
        
        err = pthread_mutex_lock(&mtx);
        
        return err == 0 ? TP_OK : TP_ERROR;
    }
    TpErrCode ThreadMutex::thread_mutex_unlock()
    {
        tp_err_t err;
        
        err = pthread_mutex_unlock(&mtx);
        
        return err == 0 ? TP_OK : TP_ERROR;
    }
    /** Thread Spin Lock */
    TpErrCode ThreadSpinLock::thread_spin_init()
    {
        tp_err_t err;
        
        err = pthread_spin_init(&spinlock, PTHREAD_PROCESS_PRIVATE);
        
        return err == 0 ? TP_OK : TP_ERROR;
    }
    TpErrCode ThreadSpinLock::thread_spin_lock()
    {
        tp_err_t err;
        
        err = pthread_spin_lock(&spinlock);
        
        return err == 0 ? TP_OK : TP_ERROR;
    }
    TpErrCode ThreadSpinLock::thread_spin_unlock()
    {
        tp_err_t err;
        
        err = pthread_spin_unlock(&spinlock);
        
        return err == 0 ? TP_OK : TP_ERROR;
    }
    TpErrCode ThreadSpinLock::thread_spin_destroy()
    {
        tp_err_t err;
        
        err = pthread_spin_destroy(&spinlock);
        
        return err == 0 ? TP_OK : TP_ERROR;
    }
    /** Thread Event */
    void CertainEvent::certain_event_handler(void *args)
    {
        TestTaskStruct* p = dynamic_cast<TestTaskStruct *>((TestTaskStruct *)args);
        // printf("event (TestTaskStruct) t_i = %d\n", p->t_i);
        // printf("event (TestTaskStruct) t_f = %f\n", p->t_f);
        // printf("event (TestTaskStruct) t_s = %s\n", p->t_s.c_str()); 
        
        return;
    }
    /** Thread Task */
    void ThreadTask::thread_task_handler(ThreadTask *task)
    {
        // printf("===== task test routine begin =====\n");
        // printf("task id = %d\n", task->id);
        TestTaskStruct* p = dynamic_cast<TestTaskStruct *>((TestTaskStruct *)task->ctx);
        // printf("task ctx(TestTaskStruct) t_i = %d\n", p->t_i);
        // printf("task ctx(TestTaskStruct) t_f = %f\n", p->t_f);
        // printf("task ctx(TestTaskStruct) t_s = %s\n", p->t_s.c_str());
        // char buf[128];
        // int n = snprintf(buf, 128, "thread #%u will handle this task", pthread_self());
        // p->t_s.assign(buf, n);
        // printf("====== task test routine end ======\n");
    }
    /** Thread Log */
    void ThreadLog::thread_log_print(TP_LOG_LEVEL lvl, const char *fmt, ...)
    {
        va_list ap;
        
        if (level >= lvl) {
            va_start(ap, fmt);
            thread_log_vprintf(lvl, fmt, ap);
            va_end(ap);
        }
    }
    
    void ThreadLog::thread_log_vprintf(TP_LOG_LEVEL lvl, const char *fmt, va_list ap)
    {
        char tbuf[64] = { 0 };
        time_t now;
        
        time(&now);
        tm *tm_now = localtime(&now);
        snprintf(tbuf, 64, "%04d.%02d.%02d %02d:%02d:%02d ",
                            tm_now->tm_year + 1900,
                            tm_now->tm_mon + 1,
                            tm_now->tm_mday,
                            tm_now->tm_hour,
                            tm_now->tm_min,
                            tm_now->tm_sec);
        fprintf(fp, "%s", tbuf);
        vfprintf(fp, fmt, ap);
        fflush(fp);
    }
    
    /** Thread Pool */
    tp_uint_t ThreadPool::m_s_task_id = 1;
    
    void * ThreadPool::thread_pool_cycle(void *userp)
    {
        ThreadPool *pool = dynamic_cast<ThreadPool *>((ThreadPool *)userp);
        
        tp_err_t        err;
        sigset_t        set;
        ThreadTask      task;

        S_TPECHO(pool, DBUG, "set signalset for thread(#%u#)", pthread_self());
    
        sigfillset(&set);

        sigdelset(&set, SIGILL);
        sigdelset(&set, SIGFPE);
        sigdelset(&set, SIGSEGV);
        sigdelset(&set, SIGBUS);
        
        err = pthread_sigmask(SIG_BLOCK, &set, nullptr);
        if (err) {
            S_TPECHO(pool, ERRO, "pthread_sigmask() failed");
            return nullptr;
        }
                
        for ( ; ; ) {
            
            S_TPECHO(pool, DBUG, "Que size=%d, Done Que size=%d, waiting=%d", 
                        pool->m_queues.size(), pool->m_queues_done.size(), pool->m_waiting);
            
            if (pool->m_mtx.thread_mutex_lock() != TP_OK) {
                return nullptr;
            }
            
            /** the number may become negative */
            pool->m_waiting--;
            
            while (pool->m_queues.empty()) {
                S_TPECHO(pool, DBUG, "wait condition signal ...");
                /** would block here if there is no task in the queue */
                if (pool->m_cond.thread_cond_wait(pool->m_mtx) != TP_OK) {
                    (void) pool->m_mtx.thread_mutex_unlock();
                    return NULL;
                }
            }
            
            /** move a task to handle from thread pool */
            task = std::move(pool->m_queues.front());
            pool->m_queues.pop();
            
            if (pool->m_mtx.thread_mutex_unlock() != TP_OK) {
                return nullptr;
            }
            
            S_TPECHO(pool, INFO, "run task #%u in thread pool \"%s\"[status: queues(%d), waiting(%d)]", 
                                    task.id, pool->m_name.c_str(), pool->m_queues.size(), pool->m_waiting);
            
            task.handler(&task);
            
            S_TPECHO(pool, INFO, "complete task #%u in thread pool \"%s\"", task.id, pool->m_name.c_str());
            
            if (pool->m_spinlock.thread_spin_lock() != TP_OK) {
                return nullptr;
            }
            
            pool->m_queues_done.push(std::move(task));
            
            tp_memory_barrier();
            
            if (pool->m_spinlock.thread_spin_unlock() != TP_OK) {
                return nullptr;
            }
            
            /// 调用同步方法对返回结果进行处理
            thread_pool_event_handler(pool);
        }
    }
    void ThreadPool::thread_pool_exit_handler(void *userp)
    {
        tp_uint_t *lock = static_cast<tp_uint_t *>((tp_uint_t *)userp);
        
        *lock = 0;
        
        pthread_exit(0);
    }
    void ThreadPool::thread_pool_event_handler(void *userp)
    {
        ThreadPool *pool = dynamic_cast<ThreadPool *>((ThreadPool *)userp);
        
        S_TPECHO(pool, DBUG, "will handle the event in the Done Que");
        
        ThreadTask      task;
        CertainEvent   *event;
        
        if (pool->m_spinlock.thread_spin_lock() != TP_OK) {
            return;
        }
        
        task = std::move(pool->m_queues_done.front());
        pool->m_queues_done.pop();
        
        tp_memory_barrier();
        
        if (pool->m_spinlock.thread_spin_unlock() != TP_OK) {
            return;
        }
        
        event = &task.event;
        
        event->complete = true;
        event->active = false;
        
        event->handler(task.ctx);
    }
    TpErrCode ThreadPool::thread_pool_init(ThreadLog &tlog)
    {
        tp_err_t        err;
        pthread_attr_t  attr;
        pthread_t       tid;
        tp_uint_t       n;
        
        TPECHO(DBUG, "thread_pool_init");
        
        std::queue<ThreadTask> empty_queue;
        m_queues.swap(empty_queue);
        
        std::queue<ThreadTask> empty_queue_done;
        m_queues_done.swap(empty_queue_done);

        err = m_mtx.thread_mutex_init();
        if (err) return TP_ERROR;
        
        err = m_cond.thread_cond_init();
        if (err) return TP_ERROR;
        
        err = m_spinlock.thread_spin_init();
        if (err) return TP_ERROR;
        
        m_log = std::move(tlog);
        
        err = pthread_attr_init(&attr);
        if (err) return TP_ERROR;
            
        /** Once cycle thread been created, detached from main thread */
        err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        if (err) {
            (void) pthread_attr_destroy(&attr);
            return TP_ERROR;
        }
        
        TPECHO(DBUG, "would create %d threads and then detach them from main thread", m_threads);
        for (n = 0; n < m_threads; n++) {
            err = pthread_create(&tid, &attr, &ThreadPool::thread_pool_cycle, this);
            if (err) return TP_ERROR;
            TPECHO(DBUG, "create thread #%u# success", tid);
        }
        
        (void) pthread_attr_destroy(&attr);
        
        return TP_OK;
    }
    TpErrCode ThreadPool::thread_pool_destroy()
    {
        TPECHO(DBUG, "xxxxxxxxxxxxxxxxxxxxxxxxxx thread_pool_destroy destroy thread #%u", pthread_self());
        
        tp_uint_t               n;
        ThreadTask              task;
        volatile tp_uint_t      lock;
        
        (void) memset(&task, 0, sizeof(task));
        
        task.handler = std::bind(&ThreadPool::thread_pool_exit_handler, std::placeholders::_1);
        task.ctx = static_cast<void *>((void *)&lock);
        
        TPECHO(DBUG, "xxxxxxxxxxxxxxxxxxxxxxxxxx thread_pool_destroy destroy thread #%u", pthread_self());
        
        for (n = 0; n < m_threads; n++) {
            lock = 1;
            
            if (thread_pool_task_post(task) != TP_OK) {
                return TP_ERROR;
            }
            
            while (lock) {
                sched_yield();
            }
            
            task.event.active = false;
        }
    
        (void) m_cond.thread_cond_destroy();
        (void) m_mtx.thread_mutex_destroy();
        (void) m_spinlock.thread_spin_destroy();
        
        return TP_OK;
    }
    TpErrCode ThreadPool::thread_pool_task_post(ThreadTask &task)
    {
        if (task.event.active) {
            return TP_ERROR;        /* already active */
        }
        
        if (m_mtx.thread_mutex_lock() != TP_OK) {
            return TP_ERROR;
        }
        
        if (m_waiting > m_maxqueue) {
            (void) m_mtx.thread_mutex_unlock(); /* current thread queue overflow */
            return TP_ERROR;
        }
        
        task.event.active = true;
        task.id = m_s_task_id++;
        TPECHO(DBUG, "post a task and send cond signal for handling");
        
        m_queues.push(std::move(task));
        
        m_waiting++;
        
        if (m_cond.thread_cond_signal() != TP_OK) {
            (void) m_mtx.thread_mutex_unlock();
            return TP_ERROR;
        }
        
        (void) m_mtx.thread_mutex_unlock();
        
        TPECHO(INFO, "task #%u added to thread pool \"%s\"[status: queues(%d), waiting(%d)]", 
                                task.id, m_name.c_str(), m_queues.size(), m_waiting);
        
        return TP_OK;
    }
};
