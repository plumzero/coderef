#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/**
    ssize_t getline(char **lineptr, size_t *n, FILE *stream);
    用法:
        读取流文件，每次读取一行，处理后再读取下一行，直至遇到 EOF 或 读取错误；
        从流中读取一行，存入lineptr中，以'\0'结尾；
        换行符也会存入lineptr中；
        若设置 *lineptr=NULL 且 *n=0，getline会为其分配动态存储，无论getline返回是否成功，用户都应free.
    返回值：
        失败 -1，并设置errno
        成功 读取行的字节数（包含换行符在内）
    注意：
        对于文本文件，换行符也是流的一部分
    argv[1]文件内容格式：
        hello world
        ABC DEF GHHHI
        GOOD
 */
int main(int argc, char *argv[])
{
    int i;
    FILE *f;
    char *line = NULL, *p = NULL;
    size_t len = 0;
    ssize_t nread;
    
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
    
    p = line;
    // 一行一行的读取
    while((nread = getline(&line, &len, f)) != -1)
    {
        printf("nread = %ld, len = %ld\n", nread, len);
        for(i = 0; i < nread; i++)
            printf(" %02X", *line++);
        printf("\n");
    }
    if (nread == -1)
    {
        if (feof(f))
            printf("end of file reached!\n");
        else
            fprintf(stderr, "getline error (%d): %s\n", errno, strerror(errno));
    }
    //注意复位
    line = p;
    
    free(line);
    fclose(f);

    return 0;
}


