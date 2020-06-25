
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
 * 通过 rand() 在很大的数值区间内获得随机数时，其效果很差。
 *
 * 本程序实现了一种 linux 下获取 64 位随机数的参考方法
 */

static inline uint64_t random64(void)
{
    uint64_t val;
    val = (uint64_t)lrand48();
    val <<= 32;
    val += (uint64_t)lrand48();
    return val;
}

int main()
{
    srand48((uint64_t)time(NULL));
    
    size_t i;
    
    for (i = 0; i < 30; i++)
    {
        printf("%lu\n", random64());
    }
    
    return 0;
}