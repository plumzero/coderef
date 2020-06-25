
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int main()
{
    char tfname[] = { "tmp_XXXXXX" };
    int fd = mkstemp(tfname);
    char ibuf[] = { "Hello World!" };
    
    printf("tmp file name=%s\n", tfname);
    /** 通过返回的描述符而不是临时文件名打开文件句柄 */
    FILE* fout = fdopen(fd, "w");
    if (! fout) {
        printf("fopen: %s\n", strerror(errno));
        return -1;
    }
    ssize_t ws = fwrite(ibuf, 1, strlen(ibuf), fout);
    if (ws < 0) {
        printf("fwrite: %s\n", strerror(errno));
        return -1;
    }
    fclose(fout);
    
    FILE* fin = fopen(tfname, "r");
    if (! fin) {
        printf("fopen: %s\n", strerror(errno));
        return -1;
    }
    unsigned char obuf[256] = {0};
    ssize_t rs = fread(obuf, 1, sizeof(obuf), fin);
    if (rs < 0) {
        printf("fread: %s\n", strerror(errno));
        return -1;
    }
    fclose(fin);
    
    printf("obuf=%s\n", obuf);
    
    unlink(tfname);
    
    
    return 0;
}
