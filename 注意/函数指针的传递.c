#include <stdio.h>
#include <stdlib.h>

int add(int a, int b)
{
    return (a+b);
}

int calc(long funcaddr)
{
    int (*func)(int, int) = (int (*)(int, int))funcaddr;
    
    int c = (*func)(10, 20);
    return c;
}

long getfunc()
{
    // int (*pFunc)(int, int) = calloc(1, sizeof(int (*)(int, int)));
    int (*pFunc)(int, int) = add;
    
    return (long)pFunc;
}

void freefunc(long funcaddr)
{
    int (*func)(int, int) = (int (*)(int, int))funcaddr;
    if (func)
    {
        printf("free me\n");
        free((void*)func);
    }
}

int main()
{
    long addr = getfunc();
    
    int d = calc(addr);
    
    printf("----%d\n", d);
    
    // freefunc(addr);
    
    return 0;
}