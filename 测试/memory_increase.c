#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/select.h>

#define COUNT 300

#define BUFSIZE  (10240 * 2)

char *p[COUNT] = { NULL };

static char data[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                       0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                       0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                       0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
};

/**
 * 观察 VmRSS 变化:
 *   VmRSS 变化量为 264 KB, 变化时间因每次分配内存不同而变化。
 * 估算方法:
 *  ( 264 - 4.125 ) / (每次分配内存量，以 KB 计) = 变化时间
 *  本例中每次分配内存量为 20 KB，则变化时间为 12.99 s
 * 注意：
 *  如果只使用 malloc 分配内存，但并不使用， VmRSS 并不会变化。
 */

void signal_handler(int signum)
{
    int i;
    switch (signum) {
        case SIGINT:
        case SIGTERM:
        {
            printf("\nSignal %d received, preparing to exit...\n", signum);
            for (i = 0; i < COUNT; i++) {
                if (p[i]) {
                    printf("信号退出，释放索引为 %d 的内存\n", i);
                    free(p[i]);
                    p[i] = NULL;
                    struct timeval timeout = { 1, 0 };
                    select(0, NULL, NULL, NULL, &timeout);
                }
            }
            signal(signum, SIG_DFL);
            exit(0);
        }
        default:
        {
            printf("Could not process this signal\n");
            break;
        }
    }
}

int main(int argc, char *argv[])
{   
    int i;
    
    if (SIG_ERR == signal(SIGINT, signal_handler)) {
        return -1;
    }
    
    printf("进程号为: %d, 父进程号为: %d\n", getpid(), getppid());
    
    for (i = 0; i < COUNT; i++) {
        printf("分配索引为 %d 的内存\n", i);
        p[i] = (char *)malloc(BUFSIZE);
        if (p[i] == NULL) {
            printf("分配索引为 %d 的内存失败\n", i);
        }
        memset(p[i], 0, BUFSIZE);
        memcpy(p[i], data, sizeof(data));
        struct timeval timeout = { 1, 0 };
        select(0, NULL, NULL, NULL, &timeout);
    }
    printf("准备释放内存...\n");
    sleep(5);
    for (i = 0; i < COUNT; i++) {
        if (p[i]) {
            printf("正常退出，释放索引为 %d 的内存\n", i);
            free(p[i]);
            p[i] = NULL;
            struct timeval timeout = { 1, 0 };
            select(0, NULL, NULL, NULL, &timeout);
        }
    }
    
    return 0;
}
