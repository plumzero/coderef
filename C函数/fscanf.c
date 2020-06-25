#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

/**
    功能：行读取一个文本文件，在stdout上输出，并写到另外一个文件
    对fscanf函数返回值的判断（一）：
        errno = 0;
        int n = 0;
        n = fscanf(f, "%[^\n]", buf);
        if (n == 1)
        {
            printf("%s\n", buf);
        }
        else if (n == EOF)
        {
            if (errno)
                fprintf(stderr, "fscanf failed. (%d): %s\n", errno, strerror(errno));
            else
                fprintf(stderr, "end of file reached.\n");
        }
    对fscanf函数返回值的判断（二）：
        errno = 0;
        int n = 0;
        n = fscanf(stdin, "%[a-z]", buf);
        if (n == 1)
        {
            printf("%s\n", buf);
        }
        else if (n == 0)
        {
            if (errno)
                fprintf(stderr, "fscanf failed. (%d): %s\n", errno, strerror(errno));
            else
                fprintf(stderr, "No matching characters.\n");
        }   
 */
int little_endian()
{
    short i = 1;
    unsigned char *p = (unsigned char*)&i;
    return (*p == 1);
}

int main(int argc, char *argv[])
{
    int i;
    FILE *f;
    char sbuf[1024];
    size_t slen = 0;
    ssize_t nw;
    int fd;
    
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <input filename> <output filename>\n", argv[0]);
        return -1;
    }
    
    if ((f = fopen(argv[1], "r")) == NULL)
    {
        perror("fopen");
        return -1;
    }
    
    if ((fd = open(argv[2], O_WRONLY| O_APPEND | O_CREAT)) == -1)
    {
        fprintf(stderr, "%d ecode(%08X): %s\n", __LINE__, errno, strerror(errno));
        goto cleanup;
    }
    while((fscanf(f, "%[^\n]", sbuf + 2)) != EOF)
    {
        fgetc(f);
        slen = (uint16_t)strlen(sbuf + 2);
        if (little_endian())
        {
            for (i = 0; i < 2; i++)
                sbuf[i] = (unsigned char)(((uint16_t)slen >> ((2 - i - 1) << 3)) & 0xFF);
        }
        else
        {
            for (i = 0; i < 2; i++)
                sbuf[i] = (unsigned char)(((uint16_t)slen >> (i << 3)) & 0xFF);
        }
        printf("%s\n", sbuf + 2);
        if (!feof(f))       //如果未到文末，打印一个换行
            strcat(sbuf + 2, "\n");
        if ((nw = write(fd, sbuf + 2, strlen(sbuf + 2))) != (ssize_t)strlen(sbuf + 2))
        {
            fprintf(stderr, "%d ecode(%08X): %s\n", __LINE__, errno, strerror(errno));
            goto cleanup;
        }
        memset(sbuf, 0, 1024);
    }
cleanup:
    fclose(f);
    close(fd);
    
    return 0;
}