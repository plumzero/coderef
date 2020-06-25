#include <stdio.h>

int my(const int a)
{
    static int count = 0;
	
    return count + a;
}

int main()
{    
    printf("%d\n%d\n%d\n", my((int)4), my((int)5), my((int)60)); // 函数入栈顺序从右向左，静态局部变量 count 在 my(60) 中完成初始化
    return 0;
}
