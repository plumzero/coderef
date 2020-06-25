

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include<unistd.h>
#include <sys/time.h>

/** 
 * 简单压测程序
 */

int test_function()
{
    return 0;
}

typedef struct BM_st {
    int m_iLoop;
    int m_iRound;
    double m_dBeginTime;
    double m_dCostTime;
} BM_st;

struct BM_st g_bm;

static double benchmark_timestamp(void)
{
    struct timeval tv;
    gettimeofday(&tv, 0);

    return (double)tv.tv_sec + (double)tv.tv_usec / 1000000;
}

static void signal_handler(int signum)
{
    switch (signum)
    {
        case SIGALRM:
            signal(signum, SIG_IGN);
            g_bm.m_iLoop = 0;
            break;
        case SIGTERM:
            signal(signum, SIG_DFL);
            break;
        case SIGINT:
            signal(signum, SIG_DFL);
            break;
        default:
            break;
    }
}

void benchmark_set(int reset)
{
    if (reset) {
        g_bm.m_iRound = 0;
        g_bm.m_iLoop = 1;
        
        alarm(3);
        g_bm.m_dBeginTime = benchmark_timestamp();
    } else {
        g_bm.m_dCostTime = benchmark_timestamp() - g_bm.m_dBeginTime;
    }
}

int main()
{
    signal(SIGALRM, signal_handler);
    
    benchmark_set(1);
    while (g_bm.m_iLoop) {
        test_function();
        g_bm.m_iRound++;
    }
    benchmark_set(0);
    printf("round=[%d] consume time=[%fs] tps=%.2f\n", 
        g_bm.m_iRound, g_bm.m_dCostTime, (double)g_bm.m_iRound / g_bm.m_dCostTime);
    
    return 0;
}