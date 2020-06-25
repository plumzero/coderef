#include <stdio.h>
#include <string.h>

int multip_buf(unsigned char **cursor, const unsigned char *start, unsigned char *str, size_t slen)
{
    if (*cursor < start || (size_t)(*cursor - start) < slen)
        return -1;
    
    (*cursor) -= slen;
    
    memcpy(*cursor, str, slen);     //因为是unsigned char类型，所以最好不要用strncpy
    
    return 0;
}

int lftsig_buf(unsigned char **cursor, const unsigned char *start, unsigned char c)
{
    if (*cursor < start || (size_t)(*cursor - start) < 1)
        return -1;
    /**
        
        (++i)前置++ 比 * 优先级高，运算步骤如下：
        1.先运算(*cursor)，计算指针位置
        2.再运算--(*cursor)，指针向前移一位
        3.最后运算*--(*cursor)，计算指针指向的变量，并给变量赋值
     */
    *--(*cursor) = c;
    
    return 0;
}

int rgtsig_buf(unsigned char **cursor, const unsigned char *start, unsigned char c)
{
    if (*cursor < start)
        return -1;
    // *++(*cursor) = c;
    /**
        (i++)后置++ 与 * 优先级相同，需要根据结合性判断，两者均是左结合性（从右向左），运算步骤如下：
        1.先运算(*cursor)，计算指针位置
        2.根据左结合性，运算*(*cursor), 计算指针指向的变量，并给变量赋值
        3.计算(*cursor)++，指针向后移一位
        注意，如果写成 **cursor++ 这样的话，相当于(*(*cursor))++，相当于将指针*cursor指向的变量加1
     */
    *(*cursor)++ = c;
    return 0;
}   
                                   
int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    
    int i = 0;
    unsigned char tbuf[10] = {0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A};
    unsigned char lbuf[10] = {0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A};
    
    unsigned char *p = tbuf + sizeof(tbuf);
    multip_buf(&p, tbuf, lbuf + 5, 5);
    for (i = 0; i < 10; i++)
        printf("%c\t", (char)tbuf[i]);  //注意转换
    printf("\n");
    
    multip_buf(&p, tbuf, lbuf, 5);
    for (i = 0; i < 10; i++)
        printf("%c\t", (char)tbuf[i]);
    printf("\n");
    
    for (i = 0; i < 5; i++)
        rgtsig_buf(&p, tbuf, 0x41 + i);
    for (i = 0; i < 10; i++)
        printf("%c\t", (char)tbuf[i]);
    printf("\n");
    
    p += 5;
    for (i = 0; i < 5; i++)
        lftsig_buf(&p, tbuf, 0x4A - i);
    for (i = 0; i < 10; i++)
        printf("%c\t", (char)tbuf[i]);
    printf("\n");
    
    return 0;
}

