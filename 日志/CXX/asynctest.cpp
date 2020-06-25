
#include "asynclog.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <thread>
#include <fstream>
#include <sys/select.h>

void thread_producer_callback(AsyncLog *al)
{
    char buf[1024];
    int total = 100000, i;
    int n;
    
    sleep(1);
    
    std::pair<std::thread::id, std::pair<AsyncLog::Level, std::string> > log_pair;
    
    for (i = 0; i < total; i++) {
        n = snprintf(buf, 1024, "%03d ======> hello world, this thread", i);
        log_pair = std::make_pair(std::this_thread::get_id(), std::make_pair(AsyncLog::INFO, std::string(buf, n)));
        *al << log_pair;
        usleep(100);        // 模拟工作时间
    }
}
/**
 * 日志线程务必早于处理线程启动
 */
void thread_consumer_callback(AsyncLog *al)
{   
    al->log_run();
}

int main()
{   
    AsyncLog al("async.log");
        
    al.log_descr();
    
    std::thread test_thread_1(thread_producer_callback, &al);
    std::thread test_thread_2(thread_producer_callback, &al);
    std::thread test_thread_3(thread_producer_callback, &al);
    std::thread test_thread_4(thread_producer_callback, &al);
    
    std::thread test_thread_0(thread_consumer_callback, &al);
    
    test_thread_1.join();
    test_thread_2.join();
    test_thread_3.join();
    test_thread_4.join();
    
    test_thread_0.join();
    
    return 0;
}