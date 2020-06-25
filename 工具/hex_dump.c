#include <stdio.h>

/**
 * 一个二进制流输出为十六进制字符串的函数
 */

void hexdumpbuf(FILE *f, unsigned char *buf, size_t len)
{
    const size_t N = 32;
    size_t i, j;

    for (i = 0; i < len;)
    {
        fputc ('[', f);
        for (j = 0; j < N && i + j < len; j++)
            fprintf (f, "%02x", buf[i + j]);
        for (; j < N; j++)
            fprintf (f, "  ");
        fputc (']', f);
        fputc ('[', f);
        for (j = 0; j < N && i + j < len; j++)
        {
            int c = buf[i + j];
            if (c < ' ' || c > 126)
                fputc ('.', f);
            else
                fputc (c, f);
        }
        fputc (']', f);
        fputc ('\n', f);
        i += j;
    }
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        printf("Usage: ./exe <infile>\n");
        return -1;
    }
    
    unsigned char buf[10240] = {0};
    size_t len;
    
    FILE* fp = fopen(argv[1], "rb");
    if (! fp) {
        printf("fopen(\"%s\") failed\n", argv[1]);
        return -1;
    }

    len = fread(buf, 1, sizeof(buf), fp);
    
    hexdumpbuf(stdout, buf, len);
    
    fclose(fp);
    
    return 0;
}