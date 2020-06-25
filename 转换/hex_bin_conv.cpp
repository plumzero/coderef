
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>

// 转 hex 时，确保写缓冲区 2 倍于读缓冲区
#define RBUFSIZE (8)
#define WBUFSIZE (RBUFSIZE * 2)

/**
 * fstream 族成员函数的使用
 * 1. 读取块文件时，需要先计算文件长度，再读取
 *    seekg + tellg + read + write
 * 2. 按行读取文件时，直接判断是否到达末尾
 *    eof + getline
 */

// 传参时，确保 obuf 缓冲初始化为 0
static int hex2bin(const unsigned char *ibuf, size_t ilen, unsigned char *obuf, size_t *olen)
{
    size_t i, j, k;
    
    for (i = 0; i < ilen; i++, ibuf++)
    {
        if (*ibuf >= '0' && *ibuf <= '9') j = *ibuf - '0'; else
        if (*ibuf >= 'A' && *ibuf <= 'F') j = *ibuf - '7'; else
        if (*ibuf >= 'a' && *ibuf <= 'f') j = *ibuf - 'W'; else
            return -1;

        k = ((i & 1) != 0) ? j : j << 4;

        obuf[i >> 1] = (unsigned char)(obuf[i >> 1] | k);
    }
    *olen = ilen / 2;
    
    return 0;
}

static int bin2hex(const unsigned char *ibuf, size_t ilen, unsigned char *obuf, size_t *olen)
{
    const unsigned char *ip, *iend;
    unsigned char *op;
    
    if (ilen > *olen / 2)
        return -1;
    
    ip = ibuf;
    iend = ip + ilen;
    op = obuf;

    for (; ip != iend; ip++) {
        *(op++) = "0123456789ABCDEF" [*ip / 16];
        *(op++) = "0123456789ABCDEF" [*ip % 16];
    }
    
    *olen = op - obuf;
    
    return 0;
}

#define USAGE  \
    "Usage: ./program <in-file> <out-file> -b|-h\n"  \
    "   -b    convert in-file to bin format\n"  \
    "   -h    convert in-file to hex format\n"

int main(int argc, char *argv[])
{
    if (argc < 4) {
        printf(USAGE);
        return -1;
    }
    
    int ret = -1;
    int tohex = 0;
    char rbuf[RBUFSIZE] = { 0 };
    char wbuf[WBUFSIZE] = { 0 };
    std::ifstream ifs;
    std::ofstream ofs;
    size_t ifsize, offset, nr, nw;
    
    if (! strcmp(argv[3], "-b")) {
        tohex = 0;
    } else if (! strcmp(argv[3], "-h")) {
        tohex = 1;
    } else {
        printf("parameter error.\n");
        goto cleanup;
    }
    
    ifs.open(argv[1], std::ifstream::in | std::ifstream::binary);
    if (! ifs.is_open()) {
        printf("open %s failed\n", argv[1]);
        goto cleanup;
    }
    
    ofs.open(argv[2], std::ofstream::out | std::ofstream::ate);
    if (! ofs.is_open()) {
        printf("open %s failed\n", argv[2]);
        goto cleanup;
    }
    
    ifs.seekg(0, ifs.end);
    ifsize = ifs.tellg();
    ifs.seekg(0, ifs.beg);
    
    printf("file(size: %lu bytes, format: %s)\n", ifsize, tohex ? "hex string" : "block");
    try {
        for (offset = 0; offset < ifsize; offset += nr) {
            nr = (ifsize - offset > RBUFSIZE) ? RBUFSIZE : (ifsize - offset);
            nw = WBUFSIZE;
            ifs.read(rbuf, nr);
            if (! tohex) {
                hex2bin((unsigned char *)rbuf, nr, (unsigned char *)wbuf, &nw);
            } else {
                bin2hex((unsigned char *)rbuf, nr, (unsigned char *)wbuf, &nw);
            }
            ofs.write(wbuf, nw);
            memset(rbuf, 0, RBUFSIZE);
            memset(wbuf, 0, WBUFSIZE);
        }
    } 
    catch (std::ios::failure e) {
        printf("read or write exception\n");
    }
    
    ret = 0;
cleanup:
    if (ret == 0) {
        printf("===== success =====\n");
    } else {
        printf("xxxxx failure xxxxx\n");
    }
    if (ifs.is_open()) ifs.close();
    if (ofs.is_open()) ofs.close();
    
    return ret; 
}
