#include <stdio.h>
#include <time.h>
#include <pthread.h>
//#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/select.h>

void go_sleep(int ms)
{
    struct timeval timeout;
    timeout.tv_sec = ms / 1000L;
    ms = ms % 1000L;
    timeout.tv_usec = (int)ms * 1000;
    select(0, NULL, NULL, NULL, &timeout);
}

void* thread1_func(void* no)
{
    (void)no;
    
    while (1) {
        printf("thread1(%d) is running...\n", (int)pthread_self());
        go_sleep(1000);
    }

    return NULL;
}

void* thread2_func(void* no)
{
    (void)no;

    while(1) {
        printf("thread2(%d) is running...\n", (int)pthread_self());
        go_sleep(1000);
    }
    
    return NULL;
}

void stop_thread(int signo)
{
    (void)signo;

    printf("exit from thread id = %d in %d seconds\n", (int)pthread_self(), 2);
    go_sleep(2000);
    exit(0);        // exit 使整个进程都退出了，所以虽然有多个线程，但这里只执行一次
}

int main()
{
    pthread_t A, B;
    
    signal(SIGINT, stop_thread);
    pthread_create(&A, NULL, thread1_func, NULL);
    pthread_create(&B, NULL, thread2_func, NULL);
    pthread_join(A, NULL);
    pthread_join(B, NULL);
    printf("thread1 id=%d, thread2 id = %d\n", (int)A, (int)B);   // 这个居然不执行
    
    return 0;
}
