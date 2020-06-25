#include <stdio.h>

#define OffSet(type, field) ((size_t)&(((type*)0)->field))

struct MyStr
{
    char a;
    int b;
    float c;
    double d;
    char e;
};

int main() {
    printf("%lu\n", OffSet(MyStr, a));
    printf("%lu\n", OffSet(MyStr, b));
    printf("%lu\n", OffSet(MyStr, c));
    printf("%lu\n", OffSet(MyStr, d));
    printf("%lu\n", OffSet(MyStr, e));
    printf("\n");

    struct MyStr str = { .a = 'a', .b = 33, .c = 3.14, .d = 2.71828, .e = 'e'};
    printf("str.a = %c, str.b = %d, str.c = %f, str.d = %lf, str.e = %c\n", str.a, str.b, str.c, str.d, str.e); 
    printf("\n");

    printf("str.a = %c, str.b = %d, str.c = %f, str.d = %lf, str.e = %c\n", 
                                    (char)*((char*)&str + OffSet(MyStr, a)), 
                                    (int)*((char*)&str + OffSet(MyStr, b)), 
                                    (float)*((char*)&str + OffSet(MyStr, c)),   // 对浮点型的取值有问题
                                    (double)*((char*)&str + OffSet(MyStr, d)), 
                                    (char)*((char*)&str + OffSet(MyStr, e)));   

    printf("str.a = %c, str.b = %d, str.c = %f, str.d = %lf, str.e = %c\n", 
                                    *((char*)((char*)&str + OffSet(MyStr, a))), 
                                    *((int*)((char*)&str + OffSet(MyStr, b))), 
                                    *((float*)((char*)&str + OffSet(MyStr, c))),    // 这样就没问题了
                                    *((double*)((char*)&str + OffSet(MyStr, d))), 
                                    *((char*)((char*)&str + OffSet(MyStr, e)))); 

    return 0;
}
