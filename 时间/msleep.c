
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

/**
 * linux 下的 sleep 函数传参单位为 秒
 *           usleep 函数传参单位为 微秒
 * 
 * 本程序实现 毫秒
 */

void msleep(long ms)
{
    struct timeval timeout;
    timeout.tv_sec = ms / 1000L;
    ms = ms % 1000L;
    timeout.tv_usec = (int)ms * 1000;
    select(0, NULL,  NULL, NULL, &timeout);
}

// 两个时间点的间隔(秒计)
double tv_diff_secs(struct timeval newer, struct timeval older)
{
    if(newer.tv_sec != older.tv_sec)
        return (double)(newer.tv_sec - older.tv_sec)+
                (double)(newer.tv_usec - older.tv_usec) / 1000000.0;
    else
        return (double)(newer.tv_usec - older.tv_usec) / 1000000.0;
}

int main()
{
    struct timeval tv1, tv2;
    gettimeofday(&tv1, NULL);
    printf("now(millisecond): %ld, then sleep for 300 milliseconds\n", 
                                        tv1.tv_sec * 1000 + tv1.tv_usec / 1000);
    
    msleep(300);
    
    gettimeofday(&tv2, NULL);
    printf("now(millisecond): %ld\n", tv2.tv_sec * 1000 + tv2.tv_usec / 1000);
    
    printf("time difference(millisecond): %.f\n", tv_diff_secs(tv2, tv1) * 1000);
    
    return 0;
}