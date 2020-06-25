
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/time.h>

#define USEC_PER_SEC             1000000L
#define TOTAL                    100000

// %#"PRIx64"
/**
 * 字节对齐测试
 *
 * packed 是效率最低的，但相差不大；
 * 默认的波动较大，还是 aligned 4 一下吧；
 */

static inline uint64_t timeval_to_us(const struct timeval *ts)
{
    return ((uint64_t) ts->tv_sec * USEC_PER_SEC) + ts->tv_usec;
}

typedef struct _default_struct {
    char c;
    int i;
    short s;
} DefaultStruct;

typedef struct _packed_struct {
    char c;
    int i;
    short s;
} __attribute__ ((packed)) PackedStruct;

typedef struct _aligned_2_struct {
    char c;
    int i;
    short s;
} __attribute__ ((aligned(2))) Aligned2Struct;

typedef struct _aligned_4_struct {
    char c;
    int i;
    short s;
} __attribute__ ((aligned(4))) Aligned4Struct;

typedef struct _aligned_8_struct {
    char c;
    int i;
    short s;
} __attribute__ ((aligned(8))) Aligned8Struct;

void test_default()
{
    int i;
    for (i = 0; i < TOTAL; i++) {
        DefaultStruct packed;
        packed.c = (char)(i % 256);
        packed.i = i * 2;
        packed.s = i;
    }
    
    return;
}

void test_packed()
{
    int i;
    for (i = 0; i < TOTAL; i++) {
        PackedStruct packed;
        packed.c = (char)(i % 256);
        packed.i = i * 2;
        packed.s = i;
    }
    
    return;
}

void test_aligned2()
{
    int i;
    for (i = 0; i < TOTAL; i++) {
        Aligned2Struct packed;
        packed.c = (char)(i % 256);
        packed.i = i * 2;
        packed.s = i;
    }
    
    return;
}

void test_aligned4()
{
    int i;
    for (i = 0; i < TOTAL; i++) {
        Aligned4Struct packed;
        packed.c = (char)(i % 256);
        packed.i = i * 2;
        packed.s = i;
    }
    
    return;
}

void test_aligned8()
{
    int i;
    for (i = 0; i < TOTAL; i++) {
        Aligned8Struct packed;
        packed.c = (char)(i % 256);
        packed.i = i * 2;
        packed.s = i;
    }
    
    return;
}

int main()
{
    struct timeval btv, etv;
    uint64_t ub, ue;

    {
        printf("===== struct default test =====\n");
        gettimeofday(&btv, NULL);
        ub = timeval_to_us(&btv);
        
        test_default();
        
        gettimeofday(&etv, NULL);
        ue = timeval_to_us(&etv);
        
        printf("len: %2lu, time diff: %" PRIu64 "\n", sizeof(DefaultStruct), ue - ub);
    }
    {
        printf("===== struct unpacked test =====\n");
        gettimeofday(&btv, NULL);
        ub = timeval_to_us(&btv);
        
        test_packed();
        
        gettimeofday(&etv, NULL);
        ue = timeval_to_us(&etv);
        
        printf("len: %2lu, time diff: %" PRIu64 "\n", sizeof(PackedStruct), ue - ub);
    }
    {
        printf("===== struct aligned 2 test =====\n");
        gettimeofday(&btv, NULL);
        ub = timeval_to_us(&btv);
        
        test_aligned2();
        
        gettimeofday(&etv, NULL);
        ue = timeval_to_us(&etv);
        
        printf("len: %2lu, time diff: %" PRIu64 "\n", sizeof(Aligned2Struct), ue - ub);
    }
    {
        printf("===== struct aligned 4 test =====\n");
        gettimeofday(&btv, NULL);
        ub = timeval_to_us(&btv);
        
        test_aligned4();
        
        gettimeofday(&etv, NULL);
        ue = timeval_to_us(&etv);
        
        printf("len: %2lu, time diff: %" PRIu64 "\n", sizeof(Aligned4Struct), ue - ub);
    }
    {
        printf("===== struct aligned 8 test =====\n");
        gettimeofday(&btv, NULL);
        ub = timeval_to_us(&btv);
        
        test_aligned8();
        
        gettimeofday(&etv, NULL);
        ue = timeval_to_us(&etv);
        
        printf("len: %2lu, time diff: %" PRIu64 "\n", sizeof(Aligned8Struct), ue - ub);
    }

    return 0;
}
