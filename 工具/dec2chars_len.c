#include <stdio.h>
#include <stdint.h>

#define ciL     (sizeof(uint32_t))      // 4
#define biL     (ciL << 3)              // 32

/**
 * 给定一个十进制数，计算其最少应占用多少个字节。
 */

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    
    size_t i;
    int bits, byts, err;
    uint32_t mask, nt;
    
    for(;;)
    {
        printf("输入一个十进制数：");
        if ((err = scanf("%d", &nt)) == 0)
        {
            printf("输入不匹配");
            while(getchar() != '\n') {}
            continue;
        }

        mask = (uint32_t)1 << (biL - 1);        // 设置掩码
        bits = biL;
        
        for (i = 0; i < biL; i++)
        {
            if (nt & mask)
                break;
            
            bits--;
            mask >>= 1;
        }
        
        byts = (bits / 8) + ((bits % 8) ? 1 : 0);
        
        printf("最少占用字节长度为：%d\n", byts);
        
    }
    
    return 0;
}


