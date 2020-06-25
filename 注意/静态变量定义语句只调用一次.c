#include <stdio.h>

void fun(int i)
{
    static int value = i++;     // 这个语句只会在第一次调用的时候执行
    value = i++;
    printf("%d\n", value);
}

int main(){
    fun(0);
    fun(1);
    fun(2); 

    return 0;
}
