#include <stdio.h>

typedef struct Data{
    int i;
}Data;

void func1(Data ** p)
{
    (*p)->i = 66;
}

void func2(Data *p)
{
    p->i = 77;
}

int main()
{
    Data d;
    Data * p = NULL;
    printf("%d---p=%p\n", __LINE__, p);
    // printf("%d---p=%x p->i=%d\n", __LINE__, p, p->i);  ¶Î´íÎó

    d.i = 55;
    p = &d;
    printf("%d---p=%p &d=%p p->i=%d d.i=%d\n", __LINE__, p, &d, p->i, d.i);
    
    func1(&p);  
    printf("%d---p=%p &d=%p p->i=%d d.i=%d\n", __LINE__, p, &d, p->i, d.i);

    func2(p);
    printf("%d---p=%p &d=%p p->i=%d d.i=%d\n", __LINE__, p, &d, p->i, d.i);
    

    return 0;
}
