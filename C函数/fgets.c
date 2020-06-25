#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/**
    char *fgets(char *s, int size, FILE *stream);
    用法：
        整个用法与 getline 相似，不过比 getline 更容易控制一些；
        读取流文件，每次读取一行，处理后再读取下一行，直至遇到 EOF 或 读取错误；
        从流中最多读取 (size - 1) 个字节，存入s，以'\0'结尾；
        换行符也会存入s中.
    返回值：
        成功：s
        失败：NULL
    注意：
        对于文本文件，换行符也是流的一部分
 */

int main(int argc, char *argv[])
{
    FILE *f;
    char buf[1024];
    size_t i, len;
    
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
    //一行一行的读取
    while ((fgets(buf, 1024, f)) != NULL)
    {
        len = strlen(buf);
        for (i = 0; i < len; i++)
            printf(" %02X", buf[i]);
        printf("\n");
    }
    
    fclose(f);

    return 0;
}