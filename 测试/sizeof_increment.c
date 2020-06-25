#include <stdio.h>

int main()
{
    int i, j = 0, k;
    i = 3;
    
    j = sizeof(++i + ++i);      /** sizeof 在编译阶段时执行，只看 i 的类型 */
    k = ++i + ++i;              /** 操作符的操作对象是内存单元，了解这些就可以得知是 k = 10 */ 
    printf("i = %d, j = %d, k = %d\n", i, j, k);    

    return 0;
}
