
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

/**
    typedef long time_t;

    struct timespec {
        time_t tv_sec;  // seconds
        long tv_nsec;   // nanoseconds
    };
    
    struct timeval {
        time_t tv_sec;  // seconds
        long tv_usec;   // microseconds
    };
 */

#define NSEC_PER_SEC             1000000000L
#define USEC_PER_SEC             1000000L

// 获取 tai 时间
static uint64_t get_tai_time()
{
    struct timeval tus;
    gettimeofday(&tus, NULL);
    
    return tus.tv_sec - 1072915200;
}

// 将纳秒转换为 timespec 结构体
static inline struct timespec ns_to_timespec(uint64_t nsec)
{
    struct timespec ts = {0, 0};

    if (nsec == 0)
        return ts;

    ts.tv_sec = nsec / NSEC_PER_SEC;
    ts.tv_nsec = nsec % NSEC_PER_SEC;

    return ts;
}
// 将 timespec 结构体转换为纳秒
static inline uint64_t timespec_to_ns(const struct timespec *ts)
{
    return ((uint64_t) ts->tv_sec * NSEC_PER_SEC) + ts->tv_nsec;
}

// 将微秒转换为 timeval 结构体
static inline struct timeval us_to_timespec(uint64_t msec)
{
    struct timeval ts = {0, 0};

    if (msec == 0)
        return ts;

    ts.tv_sec = msec / USEC_PER_SEC;
    ts.tv_usec = msec % USEC_PER_SEC;

    return ts;
}
// 将 timeval 结构体转换为微秒
static inline uint64_t timeval_to_us(const struct timeval *ts)
{
    return ((uint64_t) ts->tv_sec * USEC_PER_SEC) + ts->tv_usec;
}


int main()
{
    struct tm *st;
    char tmbuf[64] = { 0 };
    time_t curtime, curtime64;
    
    // 参照
    time(&curtime);
    st = localtime(&curtime);
    strftime(tmbuf, sizeof(tmbuf), "%Y-%m-%d %H:%M:%S", st);
    printf("%s\n\n", tmbuf);
    printf("sec = %ld\n", curtime);
    
    // 纳秒
    struct timespec tns, tns_r;
    clock_gettime(CLOCK_REALTIME, &tns);
    curtime64 = timespec_to_ns(&tns);
    tns_r = ns_to_timespec(curtime64);
    printf("sec = %ld, nsec = %ld\n", tns_r.tv_sec, tns_r.tv_nsec);
    
    // 微秒
    struct timeval tus, tus_r;
    gettimeofday(&tus, NULL);
    curtime64 = timeval_to_us(&tus);
    tus_r = us_to_timespec(curtime64);
    printf("sec = %ld, usec = %ld\n", tus_r.tv_sec, tus_r.tv_usec);
    
    return 0;
}
