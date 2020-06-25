
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int main()
{
    char tfname[256] = {0};
    tmpnam(tfname);
    char ibuf[] = { "Hello World!" };
    
    printf("tmp file name=%s\n", tfname);
    
    FILE* fout = fopen(tfname, "w");
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