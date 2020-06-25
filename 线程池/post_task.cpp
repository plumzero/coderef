
#include "threadpool.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

using namespace ThreadPoolNS;

#define BUFSIZE  128

int main()
{
    ThreadLog  tlog;
    ThreadPool tp;
    
    int i, n;
    
    tp.thread_pool_init(tlog);
    
    // 暂时还无法良好的关闭
    for (i = 0; i < 500; i++) {
        ThreadTask task;
        TestTaskStruct *p = (TestTaskStruct*)calloc(1, sizeof(TestTaskStruct));
        p->t_i = i + 10;
        p->t_f = i + 100.99f;
        p->t_s.assign("hello world");
        task.ctx = static_cast<void*>((void *)p);
        tp.thread_pool_task_post(task);
    }
    
    pthread_t pid = pthread_self();
    
    
    pthread_join(pid, NULL);
    
    // tp.thread_pool_destroy();
    
    // sleep(4);
    
    return 0;
}

