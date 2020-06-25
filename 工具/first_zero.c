
#include <stdio.h>

/**
 * 找到第一次出现字符大小为0的位置的方法
 */

int main()
{
    size_t i, first = 0, found = 0;
    unsigned char arr[] = { 8, 67, 19, 0, 189, 254, 0, 73, 226 };
    unsigned char *p;
    
    p = arr;
    
    for (i = 0; i < (sizeof(arr) / sizeof(*arr)); i++)
    {
        found |= ((p[i] | (unsigned char) - p[i]) >> 7) ^ 1;
        first += ((found | (unsigned char) - found) >> 7) ^ 1;
        
        printf("p[%lu]=%-3u, (unsigned char)-p[%lu]=%-3u, found=%-3lu, first=%-3lu\n", 
                    i, p[i], i, (unsigned char)-p[i], found, first);
    }
    
    printf("\first = %lu\n", first);
    
    return 0;
}