
#include "threadpool.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

/**
 * ThreadPool::ThreadCond 和 ThreadPool::ThreadMutex 类的测试函数 
 *
 * g++ -g tp_cond_mutex.cpp threadpool.cpp -o tp_cond_mutex -std=c++11 -lpthread
 */

void msleep(long ms)
{
    struct timeval timeout;
    timeout.tv_sec = ms / 1000L;
    ms = ms % 1000L;
    timeout.tv_usec = (int)ms * 1000;
    select(0, NULL,  NULL, NULL, &timeout);
}

double tv_diff_secs(struct timeval newer, struct timeval older)
{
    if(newer.tv_sec != older.tv_sec)
        return (double)(newer.tv_sec - older.tv_sec)+
                (double)(newer.tv_usec - older.tv_usec) / 1000000.0;
    else
        return (double)(newer.tv_usec - older.tv_usec) / 1000000.0;
}
 
#define WORKLOAD  50000                 // 如果队列太大的话，还是用动态内存吧

// 可以将生产者、消费者时间调整后再作测试
#define PRODUCETIME     250             // 生产者生产时间  600  500
#define CONSUMETIME     250             // 消费者消费时间  200  500
 
ThreadPoolNS::ThreadMutex mutex;
ThreadPoolNS::ThreadCond cond;


const int workload = WORKLOAD;          // 要求工作量到达 WORKLOAD 停止
int queue[WORKLOAD] = { 0 };            // 生产者消费者队列

int idx_p = 0;
int idx_c = 0;

void *thread_producer(void *arg)
{
    printf("producer start to work...\n");
    // int i = 0;
    int over = 0;   // 消费者是否消费完毕
    int count = 0;
    
    while (idx_p < workload) {
        mutex.thread_mutex_lock();
        if (queue[idx_p] == 0) {                // 生产者进行生产
            printf("Producing goods(id = %d) ... \n", idx_p);
            queue[idx_p++] = 1;
        } else {
            printf("produce exception");
        }
        // msleep(PRODUCETIME);             // 模拟生产者生产时间
        cond.thread_cond_signal();          // 锁被消费者获得，生产者继续向下执行
        mutex.thread_mutex_unlock();        // 生产者等待消费者将锁释放
        while (idx_p - idx_c > 10) {        // 生产者消费者处理索引相差太大时，生产者不断催促消费者
            cond.thread_cond_signal();
        }
    }
    
    while (! over) {                        // 消费者需要生产者(信号)的催促，不然就怠工了
        count = 0;
        for (idx_p = 0; idx_p < workload; idx_p++) {
            if (queue[idx_p] == 1) {
                count++;
                break;
            }
        }
        if (count) {                        // 未消费完全，继续发送信号
            cond.thread_cond_signal();
            // msleep(100);                 // 为了节省 cpu 资源 ...
        } else {
            over = 1;
        }
    }
}

void *thread_consumer(void *arg)
{
    printf("consumer start to work...\n");
    
    // int i = 0;
    while (idx_c < workload) {
        mutex.thread_mutex_lock();
        printf("consumer is waiting for cond\n");
        cond.thread_cond_wait(mutex);       // 消费者等待生产者发送信号
        mutex.thread_mutex_unlock();        // 消费者收到信号，得到锁。消费者会立即将锁给生产者，以便其继续生产。
        if (queue[idx_c] == 1) {            // 消费者进行消费
            printf("Consuming goods(id = %d) ... \n", idx_c);
            queue[idx_c++] = 2;
        } else {
            printf("consume exception");
        }
        // msleep(CONSUMETIME);             // 模拟消费者消费时间
    }
}

int main()
{
    pthread_t pid_producer, pid_consumer;
    int i;
    int count = 0;
    struct timeval btv, etv;
    
    cond.thread_cond_init();
    mutex.thread_mutex_init();
    
    gettimeofday(&btv, NULL);
    pthread_create(&pid_consumer, NULL, thread_consumer, (void*)NULL);
    pthread_create(&pid_producer, NULL, thread_producer, (void*)NULL);
    
    pthread_join(pid_producer, NULL);
    pthread_join(pid_consumer, NULL);
    
    gettimeofday(&etv, NULL);
    
    cond.thread_cond_destroy();
    mutex.thread_mutex_destroy();
        
    for (i = 0; i < WORKLOAD; i++) {
        if (queue[i] == 2)
            count++;
    }
    if (count == workload) {
        printf("========== success ==========\n");
        printf("time cost(millisecond): %.f\n", tv_diff_secs(etv, btv) * 1000);
    } else {
        printf("xxxxxxxxxx failure xxxxxxxxxx\n");
    }
    
    return 0;
}