#include <stdio.h>

int main()
{
    int array[2][2][3] = {{{1, 2, 3}, {4, 5, 6}}, {{7, 8, 9}, {10, 11, 12}}};
    
    int i;
    for (i = 0; i < 12; i++) {
        printf("%d ", array[i/6][(i/3)%2][i%3]);
    }
    printf("\n");

    int* p = &array[0][0][0];   // p 指针指向单个个体的地址，三维数组是按顺序存储的，p 指针每增1，即向后增加一个sizeof(int) 长度
    printf("*(p + 5) = %d\n", *(p + 5));    
    printf("*(p + 10) = %d\n", *(p + 10));

    p = &array[1][1][1];
    printf("*(p + 1) = %d\n", *(p + 1));
    printf("*(p - 5) = %d\n", *(p - 5));

    return 0;
}
