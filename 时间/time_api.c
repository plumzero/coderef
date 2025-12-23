#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define ITERATIONS 10000000  // 1000万次，足够统计意义

// 精确计时函数（使用clock_gettime）
static inline double get_time_ns() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1e9 + ts.tv_nsec;
}

int main() {
    struct timeval tv;
    double start, end;
    int i;
    
    printf("=== 时间获取性能测试 ===\n\n");
    
    // 预热缓存
    for (i = 0; i < 1000; i++) {
        gettimeofday(&tv, NULL);
    }
    
    // 测试1: gettimeofday（VDSO优化）
    start = get_time_ns();
    for (i = 0; i < ITERATIONS; i++) {
        gettimeofday(&tv, NULL);
    }
    end = get_time_ns();
    printf("gettimeofday (VDSO) 性能:\n");
    printf("  总时间: %.2f ms\n", (end - start) / 1e6);
    printf("  每次调用: %.2f ns\n\n", (end - start) / ITERATIONS);
    
    // 测试2: getppid（总是系统调用）
    start = get_time_ns();
    for (i = 0; i < ITERATIONS; i++) {
        getppid();
    }
    end = get_time_ns();
    printf("getppid (系统调用) 性能:\n");
    printf("  总时间: %.2f ms\n", (end - start) / 1e6);
    printf("  每次调用: %.2f ns\n\n", (end - start) / ITERATIONS);
    
    // 测试3: time()函数（VDSO优化）
    start = get_time_ns();
    for (i = 0; i < ITERATIONS; i++) {
        time(NULL);
    }
    end = get_time_ns();
    printf("time() (VDSO) 性能:\n");
    printf("  总时间: %.2f ms\n", (end - start) / 1e6);
    printf("  每次调用: %.2f ns\n\n", (end - start) / ITERATIONS);
    
    // 测试4: clock_gettime（不同时钟源）
    struct timespec ts;
    
    // CLOCK_REALTIME_COARSE（最轻量）
    start = get_time_ns();
    for (i = 0; i < ITERATIONS; i++) {
        clock_gettime(CLOCK_REALTIME_COARSE, &ts);
    }
    end = get_time_ns();
    printf("clock_gettime(COARSE) 性能:\n");
    printf("  每次调用: %.2f ns\n\n", (end - start) / ITERATIONS);
    
    // CLOCK_REALTIME（正常）
    start = get_time_ns();
    for (i = 0; i < ITERATIONS; i++) {
        clock_gettime(CLOCK_REALTIME, &ts);
    }
    end = get_time_ns();
    printf("clock_gettime(REALTIME) 性能:\n");
    printf("  每次调用: %.2f ns\n", (end - start) / ITERATIONS);
    
    return 0;
}