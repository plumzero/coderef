
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <getopt.h>
#include <errno.h>
#include <limits.h>

/**
 * UTC 时间下（比北京时间慢 8 个小时）:
 *   Epoch 时间: 自 1970-01-01 子夜至现在的秒数
 *   Tai   时间: 自 2004-01-01 子夜至现在的秒数
 * Epoch 时间、Tai 时间、钟表时间均是以秒为单位，三者互相的转换是没有误差的。
 *
 * 本转换使用北京时间。
 */

int tai_to_other(uint64_t tai_time, time_t *epoch_time, char *clock_time, size_t len)
{
    int ret = -1;
    struct tm *st;
    char tmbuf[64] = { 0 };
    
    *epoch_time = tai_time + 1072915200;
    
    st = localtime(epoch_time);
    strftime(tmbuf, sizeof(tmbuf), "%Y-%m-%d %H:%M:%S", st);
    if (len < strlen(tmbuf)) {
        fprintf(stderr, "buffer is not enough\n");
        goto cleanup;
    }
    memcpy(clock_time, tmbuf, strlen(tmbuf));
    
    ret = 0;
cleanup:
    
    return ret;
}

int epoch_to_other(time_t epoch_time, uint64_t *tai_time, char *clock_time, size_t len)
{
    int ret = -1;
    struct tm *st;
    char tmbuf[64] = { 0 };
        
    *tai_time = epoch_time - 1072915200;
    
    st = localtime(&epoch_time);
    strftime(tmbuf, sizeof(tmbuf), "%Y-%m-%d %H:%M:%S", st);
    if (len < strlen(tmbuf)) {
        fprintf(stderr, "buffer is not enough\n");
        goto cleanup;
    }
    memcpy(clock_time, tmbuf, strlen(tmbuf));
    
    ret = 0;
cleanup:
    
    return ret;
}

int clock_to_other(char *clock_time, size_t len, time_t *epoch_time, uint64_t *tai_time)
{
    (void)len;
    
    int ret = -1;
    int year, month, day, hour, minute, second;
    struct tm ts;
    
    year = month = day = hour = minute = second = 0;
    
    ret = sscanf(clock_time, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
    if (ret != 6) {
        fprintf(stderr, "timestamp format error(ret: %d)\n", ret);
        goto cleanup;
    }
    
    if (year < 1970) {
        fprintf(stderr, "year error, %04d not in [1970, ...)\n", year);
        goto cleanup;
    }
    if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12) {
        if (day < 0 || day > 31) {
            fprintf(stderr, "%04d-%02d-%02d error\n", year, month, day);
            goto cleanup;
        }
    } else if (month == 4 || month == 6 || month == 9 || month == 11) {
        if (day < 0 || day > 30) {
            fprintf(stderr, "%04d-%02d-%02d error\n", year, month, day);
            goto cleanup;
        }
    } else if (month == 2) {
        if((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0 && year % 3200 != 0) || year % 172800 == 0) {
            if (day < 0 || day > 29) {
                fprintf(stderr, "%04d-%02d-%02d error\n", year, month, day);
                goto cleanup;
            }
        } else {
            if (day < 0 || day > 28) {
                fprintf(stderr, "%04d-%02d-%02d error\n", year, month, day);
                goto cleanup;
            }
        }
    } else {
        fprintf(stderr, "month error, %02d not in [1, 12]\n", month);
        goto cleanup;
    }
    if (hour < 0 || hour > 23) {
        fprintf(stderr, "hour error, %02d not in [0, 23]\n", hour);
        goto cleanup;
    }
    if (minute < 0 || minute > 59) {
        fprintf(stderr, "minute error, %02d not in [0, 59]\n", minute);
        goto cleanup;
    }
    if (second < 0 || second > 59) {
        fprintf(stderr, "second error, %02d not in [0, 59]\n", second);
        goto cleanup;
    }
    
    ts.tm_year = year - 1900;
    ts.tm_mon  = month - 1;
    ts.tm_mday = day;
    ts.tm_hour = hour;
    ts.tm_min  = minute;
    ts.tm_sec  = second;
    
    *epoch_time = mktime(&ts);
    *tai_time = (uint64_t)*epoch_time - 1072915200;
    
    ret = 0;
cleanup:
    
    return ret;
}

int now_to_other(char *clock_time, size_t len, time_t *epoch_time, uint64_t *tai_time)
{
    int ret = -1;
    struct timeval tus;
    struct tm *st;
    char tmbuf[64] = { 0 };
    
    ret = gettimeofday(&tus, NULL);
    if (ret == -1) {
        fprintf(stderr, "gettimeofday failure: %s\n", strerror(errno));
        goto cleanup;
    }
    
    st = localtime(&tus.tv_sec);
    strftime(tmbuf, sizeof(tmbuf), "%Y-%m-%d %H:%M:%S", st);
    if (len < strlen(tmbuf)) {
        fprintf(stderr, "buffer is insufficient\n");
        goto cleanup;
    }
    memcpy(clock_time, tmbuf, strlen(tmbuf));
    
    *epoch_time = tus.tv_sec;
    *tai_time = (uint64_t) tus.tv_sec - 1072915200;
        
    ret = 0;
cleanup:
    
    return ret;
}

#define USAGE  \
    "Usage: ./program [options] <parameter> ...\n"  \
    "   -c val      Clock time format(beijing time not UTC time)\n"  \
    "   -e val      Epoch time format\n"  \
    "   -t val      Tai time format\n"  \
    "   -n          get current time with 3 format\n"  \
    "For example:\n"  \
    "   ./program -n\n"  \
    "   ./program -c \"2019-11-15 18:22:08\"\n"  \
    "   ./program -e 1573813328\n"  \
    "   ./program -t 500898128\n"
    
int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, USAGE);
        return -1;
    }
    
    int ret = -1;
    int option = 0;
    char clock_time[64] = { 0 };
    time_t epoch_time = 0;
    uint64_t tai_time = 0;
    
    while ((option = getopt(argc, argv, "c:e:t:n")) != -1) {
        switch (option) {
            case 'c':       // clock time format(2019-11-15 17:25:30 for example)
                memcpy(clock_time, optarg, strlen(optarg));
                ret = clock_to_other(clock_time, strlen(clock_time), &epoch_time, &tai_time);
                if (ret != 0) {
                    goto cleanup;
                }
                break;
            case 'e':       // Epoch time format
                errno = 0;
                epoch_time = strtol(optarg, NULL, 10);
                if ((errno == ERANGE && (epoch_time == LONG_MAX || epoch_time == LONG_MIN))
                            || (errno != 0 && epoch_time == 0)) {
                   fprintf(stderr, "strtol failure: %s\n", strerror(errno));
                   goto cleanup;
                }   
                ret = epoch_to_other(epoch_time, &tai_time, clock_time, sizeof(clock_time));
                if (ret != 0) {
                    goto cleanup;
                }
                break;
            case 't':       // Tai time format
                errno = 0;
                tai_time = strtol(optarg, NULL, 10);
                if ((errno == ERANGE && (tai_time == LONG_MAX || tai_time == (uint64_t)LONG_MIN))
                            || (errno != 0 && tai_time == 0)) {
                   fprintf(stderr, "strtol failure: %s\n", strerror(errno));
                   goto cleanup;
                }   
                ret = tai_to_other(tai_time, &epoch_time, clock_time, sizeof(clock_time));
                if (ret != 0) {
                    goto cleanup;
                }
                break;
            case 'n':
                ret = now_to_other(clock_time, sizeof(clock_time), &epoch_time, &tai_time);
                if (ret != 0) {
                    goto cleanup;
                }
                break;
            default:
                fprintf(stderr, USAGE);
                return -1;
        }
    }
    if (ret) {
        fprintf(stderr, USAGE);
        return -1;
    }
    
    printf("clock time: %s\n", clock_time);
    printf("epoch time: %lu\n", (uint64_t)epoch_time);
    printf("tai   time: %ld\n", tai_time);
    
    ret = 0;
cleanup:
    
    return ret;
}