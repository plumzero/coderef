#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/**
 功能：
    利用欧氏算法求解给定 m 和 n 时的 s 和 t    m × s + n × t = d (d 为 m 和 n 的最大公约数)
 注意：
    1.递归函数中尽量不要进行数值的替换；
    2.尾递归可以尝试使用循环代替。反之，循环可以尝试转化为尾递归；
    3.euclid 不是尾递归，无法转化为循环结构
 */
// b = mq + r
int euclid(uint32_t b, uint32_t m, uint32_t * d, int32_t * s, int32_t * t)
{
    // 前向计算
    uint32_t q, r;
    q = b / m;
    r = b % m;
    
    if (r == 0)
    {
        *d = m;
        *s = 0;
        *t = 1;
        if (*d != 1) printf("最大公约数为 %d\n", *d);
        return 0;
    }
    
    euclid(m, r, d, s, t);  // 不要进行数值替换
    
    // 后向计算
    int32_t i = *t;
    *t = -1 * i * q + *s;
    *s = i;
    
    return 0;
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    
    uint32_t m, n, d; // d 是最大公约数
    int32_t s, t;
    int err;
    
    for (;;)
    {
        printf("输入要判断的两个非负整数(十进制，以空格隔开)：");
        err = scanf("%u %u", &m, &n);
        if (err < 2)
        {
            printf("输入格式不匹配\n");
            while(getchar() != '\n') {}
            continue;
        }
        if (m == 0 || n == 0)
        {
            printf("输入数值有误\n");
            continue;
        }
        if (m < n)
        {
            m = m ^ n;
            n = m ^ n;
            m = m ^ n;
        }
        // 求最大公约数
        euclid(m, n, &d, &s, &t);
        
        if (d == 1) printf("%u 和 %u 互素\n", m, n);
        
        printf("(%d) × %d + (%d) × %d = %d\n", t, n, s, m, d);
    }

    return 0;
}