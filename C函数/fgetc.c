#include <stdio.h>
#include <errno.h>

/**
    int fgetc(FILE *stream);
    用法:
        从流中读取一个字节，之后文件指针自动后移一位；
        根据读取结果设置结束标志，否则会读至 EOF；
        读取的字节会从unsigned char转为int 或 EOF.
    返回值：
        成功 字节的int型或EOF
        失败 返回EOF，并设置errno
    注意：
        对于文本文件，换行符也是流的一部分
    argv[1]文件内容格式：
        hello world
        ABC DEF GHHHI
        GOOD
 */
int main(int argc, char *argv[])
{
    FILE *f;
    int c;
    
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return -1;
    }
    
    if ((f = fopen(argv[1], "r")) == NULL)
    {
        perror("fopen");
        return -1;
    }
    
    //只读第一行
    while ((c = fgetc(f)) != '\n')
    {
        printf(" %02X", (int)c);
    }
    printf("\n");

    fseek(f, 0, SEEK_SET);
    //只到文末
    while ((c = fgetc(f)) != EOF)
    {
        printf(" %02X", (int)c);
    }
    printf("\n");
    
    fclose(f);
    
    return 0;
}