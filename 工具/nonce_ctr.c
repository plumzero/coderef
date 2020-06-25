#include <stdio.h>
#include <string.h>

/**
 *  nonce 控制。
 *  a 是一个字节数组，可看作一个大整数。 a 每调用一次 nonce, 则其值递增 1。
 *  a 的递增索引范围为 [lowidx, upidx]
 */
int nonce(unsigned char a[], int lowidx, int upidx)
{
    if (lowidx > upidx)
        return -1;
    if ((unsigned char)(a[upidx] ^ 0xFF) != 0)
    {
        ++a[upidx];
        return 0;
    }
    else
    {
        a[upidx] ^= 0xFF;
        return nonce(a, lowidx, upidx - 1);
    }
}

int main()
{
    size_t i;
    unsigned char a[8] = {252, 253, 255, 255, 255, 255, 255, 255};
    int len = sizeof(a) / sizeof(a[1]);
    
    
    for (i = 0; i < sizeof a; i++)
        printf("a[%lu]=%-3d  ", i, a[i]);
    printf("\n");
    
    nonce(a, 0, len - 1);
    for (i = 0; i < sizeof a; i++)
        printf("a[%lu]=%-3d  ", i, a[i]);
    printf("\n");
    
    nonce(a, 0, len - 1);
    nonce(a, 0, len - 1);
    for (i = 0; i < sizeof a; i++)
        printf("a[%lu]=%-3d  ", i, a[i]);
    printf("\n");
    
    return 0;
}
