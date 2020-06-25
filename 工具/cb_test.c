#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/** 测试结构体 */
typedef struct _test_string {
    unsigned char *buf;
    size_t        size;
} TEST_STRING;


typedef int(write_cb)(const void *buffer, size_t size, void *user_data);
typedef int(read_cb)(void *buffer, size_t size, void *user_data);

/** 自定义写回调函数 */
int write_callback(const void *buffer, size_t size, void *user_data)
{
    FILE *fp = (FILE *)user_data;
    
    return fwrite(buffer, 1, size, fp);
}

/** 自定义读回调函数 */
int read_callback(void *buffer, size_t size, void *user_data)
{   
    FILE *fp = (FILE *)user_data;
    
    return fread(buffer, 1, size, fp);
}

/** 写回调调用示例函数 */
int write_operation(const void* sptr, write_cb *cb, void *user_data)
{
    const TEST_STRING *st = (const TEST_STRING*)sptr;

    return cb(st->buf, st->size, user_data);
}

/** 读回调调用示例函数 */
int read_operation(const void* sptr, read_cb *cb, void *user_data)
{
    TEST_STRING *st = (TEST_STRING *)sptr;
    
    return cb(st->buf, st->size, user_data);
}

/** 单次测试 */
int callback_single_test()
{
    FILE *rfp = NULL;
    FILE *wfp = NULL;
    unsigned char rbuf[2048] = { 0 };
    size_t rsize, wsize;
    TEST_STRING ts = { NULL, 0 };
    
    ts.buf = rbuf;
    ts.size = 2048;
    
    /// 读测试
    rfp = fopen("cb_test.c", "rb");
    rsize = read_operation(&ts, read_callback, rfp);
    printf("read %lu bytes from %s\n", rsize, "cb_test.c");
    /// 写测试
    ts.size = rsize;
    wfp = fopen("write.txt", "wb");
    wsize = write_operation(&ts, write_callback, wfp);
    printf("write %lu bytes to %s\n", wsize, "write.txt");
    /// 是否成功
    if (wsize == rsize) {
        printf("========== callback test success ==========\n");
    } else {
        printf("xxxxxxxxxx callback test failed xxxxxxxxxx\n");
    }
    
    return 0;
}


/**
 * 记得在哪里看到的，对同一个文件描述符的引用计数不能大于 64，现在
 * 测试的是 total 数量，希望对描述符的引用计数能及时释放。
 */

/**
 *  回调测试 100000 次用时 5 s
 */
int callback_performance_test()
{
    int total = 100000;
    int count = 0;
    int i;
    time_t btime, etime;

    btime = time(NULL);
    for (i = 0; i < total; i++) {
        unsigned char *rptr = NULL;
        FILE *rfp = NULL;
        FILE *wfp = NULL;
        size_t rsize, wsize;
        char pbuf[256] = { 0 };
        int ret;
        TEST_STRING ts = { NULL, 0 };
        
        rptr = (unsigned char *)calloc(1, 8192);
        if (rptr == NULL) {
            printf("calloc failed(i = %d)\n", i);
            goto internal_cleanup;
        }
        ts.buf = rptr;
        ts.size = 8192;
        
        rfp = fopen("cb_test.c", "rb");
        if (rfp == NULL) {
            printf("fopen('rb') failed(i = %d)\n", i);
            goto internal_cleanup;
        }
        
        ret = snprintf(pbuf, 256, "write_%05d.txt", i);
        if (ret < 0 || ret > 255) {
            printf("snprintf failed(i = %d)\n", i);
            goto internal_cleanup;
        }
        
        wfp = fopen(pbuf, "wb");
        if (wfp == NULL) {
            printf("fopen('wb') failed(i = %d)\n", i);
            goto internal_cleanup;
        }
        
        rsize = read_operation(&ts, read_callback, rfp);
        
        ts.size = rsize;
        
        wsize = write_operation(&ts, write_callback, wfp);
        if (wsize == rsize) {
            count++;
        }
    internal_cleanup:
        if (rptr) free(rptr);
        if (rfp) fclose(rfp);
        if (wfp) fclose(wfp);
    }
    
    etime = time(NULL);
    
    if (count == total) {
        printf("========== test success ==========\n");
        printf("info: cost %d seconds\n", (int)(etime - btime));
    } else {
        printf("xxxxxxxxxx test failed xxxxxxxxxx\n");
        printf("error info: %d != %d\n", total, count);
    }
    
    return 0;
}
/**
 * 参照测试
 */
int contrast_performance_test()
{
    int total = 100000;
    int count = 0;
    int i;
    time_t btime, etime;

    btime = time(NULL);
    for (i = 0; i < total; i++) {
        unsigned char *rptr = NULL;
        FILE *rfp = NULL;
        FILE *wfp = NULL;
        size_t rsize, wsize;
        char pbuf[256] = { 0 };
        int ret;
        TEST_STRING ts = { NULL, 0 };
        
        rptr = (unsigned char *)calloc(1, 8192);
        if (rptr == NULL) {
            printf("calloc failed(i = %d)\n", i);
            goto internal_cleanup;
        }
        ts.buf = rptr;
        ts.size = 8192;
        
        rfp = fopen("cb_test.c", "rb");
        if (rfp == NULL) {
            printf("fopen('rb') failed(i = %d)\n", i);
            goto internal_cleanup;
        }
        
        ret = snprintf(pbuf, 256, "write_%05d.txt", i);
        if (ret < 0 || ret > 255) {
            printf("snprintf failed(i = %d)\n", i);
            goto internal_cleanup;
        }
        
        wfp = fopen(pbuf, "wb");
        if (wfp == NULL) {
            printf("fopen('wb') failed(i = %d)\n", i);
            goto internal_cleanup;
        }
        
        rsize = fread(ts.buf, 1, ts.size, rfp);
        
        ts.size = rsize;
        
        wsize = fwrite(ts.buf, 1, ts.size, wfp);
        if (wsize == rsize) {
            count++;
        }
    internal_cleanup:
        if (rptr) free(rptr);
        if (rfp) fclose(rfp);
        if (wfp) fclose(wfp);
    }
    
    etime = time(NULL);
    
    if (count == total) {
        printf("========== test success ==========\n");
        printf("info: cost %d seconds\n", (int)(etime - btime));
    } else {
        printf("xxxxxxxxxx test failed xxxxxxxxxx\n");
        printf("error info: %d != %d\n", total, count);
    }
    
    return 0;
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    
    // callback_single_test();
    callback_performance_test();
    // contrast_performance_test();
        
    return 0;
}