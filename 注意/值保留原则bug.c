#include <stdio.h>

int array[] = {23, 34, 12, 17, 204, 99, 16};
#define TOTAL_ELEMENTS  (sizeof(array)/sizeof(array[0]))

int main()
{
    int d = -1, x = 0;
    // if (d <= TOTAL_ELEMENTS - 2)     //TOTAL_ELEMENTS为unsigned int类型，左边的d会被提升为一个超大数，表达式结果为false
    if (d <= (int)TOTAL_ELEMENTS - 2)   //要进行强制类型转换，才能得到期望结果
        x = array[d+1];
    printf("x = %d\n", x);
}
