#include <stdio.h>
#include <stdint.h>

//0-大端 1-小端
int little_endian()
{
    short i = 1;
    unsigned char *p = (unsigned char*)&i;
    return (*p == 1);
}

//  0x1 0x2 0x3 0x4 0x5 0x6 0x7 0x8
//  p   p                   p+6 p+7
long to_big_endian(long llv)
{
    if (!little_endian())
        return llv;
    
    size_t i;
    unsigned char *p = (unsigned char *)llv, *q = p + 7, *t;
    //对整型内部进行内存的写操作是不被允许的
    for (i = 0; i < sizeof(long) / 2; i++)
    {
        *t = *p;
        *p = *q;
        *q = *t;
        p++;
        q--;
    }
    return llv;
}
/**
 1.与大小端无关，系统总是按内存地址从低到高的顺序读取数据的；
 2.与大小端无关，>>与减址方向相同，<<与增址方向相同；
 3.为了视觉上的移位方便，可以写成这样：
    小端模式
        高地址     <-      低地址
        高位字节    <-      低位字节
    大端模式
        高地址     <-      低地址
        低位字节    <-      高位字节
 */     
int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    
    int i, j, tlen;
    unsigned char buf[100] = {0};
    uint64_t lbv, llv = 0x07424344454647;
    
    // printf("long %d int %d short %d\n", sizeof(long), sizeof(int), sizeof(short));
    
    char *cflg = NULL;
    if (little_endian())
        cflg = "小端模式";
    else
        cflg = "大端模式";
    
    //大端模式的处理过程与小端完全相同
    printf("%s\n", cflg);
    tlen = (int)sizeof(long);
    printf("long 型 %lu\n", llv);
    //从低地址开始存起
    for (i = 0; i < tlen; i++)
        buf[i] = (unsigned char)((llv >> (i << 3)) & 0xFF);
    printf("%s内存排列（高地址<-低地址）：\n", cflg);
    for (j = tlen - 1; j >= 0; j--)
        printf("%02X ", (unsigned char)buf[j]);
    printf("\n");
    //模式转换
    printf("%s转换：\n", cflg);
    lbv = 0;
    //先移位至低地址处进行处理，后移位至相应位置
    for (i = 0; i < tlen; i++)
        lbv |= ((llv >> (i << 3)) & 0xFF) << ((tlen - i - 1) << 3);
    printf("long 型 %lu\n", lbv);
    for (i = 0; i < tlen; i++)
        buf[i] = (unsigned char)((lbv >> (i << 3)) & 0xFF);
    printf("%s转换后内存排列（高地址<-低地址）：\n", cflg);
    for (j = tlen - 1; j >= 0; j--)
        printf("%02X ", (unsigned char)buf[j]);
    printf("\n");
    
    //也可以进行其他类型的大小端转换...
    
    return 0;
}