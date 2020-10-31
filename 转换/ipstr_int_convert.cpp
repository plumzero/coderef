
#include <stdio.h>
#include <stdint.h>
#include <arpa/inet.h>

int main()
{
    int ret = -1;
    const char *str = "192.168.1.5";
    
    unsigned int a, b, c, d;
    ret = sscanf(str, "%d.%d.%d.%d", &a, &b, &c, &d);
    if (ret != 4) {
        printf("sscanf error(%d)\n", ret);
        return -1;
    }
    uint32_t m = htonl(((uint32_t) a << 24) | ((uint32_t) b << 16) | c << 8 | d);   // 转网络序
    
    printf("a=%d,b=%d,c=%d,d=%d\n", a, b, c, d);
    printf("m=%u\n\n", m);
    
    uint32_t n = ntohl(m);          // 转主机序
    
    printf("n=%u\n", n);
    
    unsigned int A, B, C, D;
    A = (unsigned char)((n >> 24) & 0xFF);
    B = (unsigned char)((n >> 16) & 0xFF);
    C = (unsigned char)((n >>  8) & 0xFF);
    D = (unsigned char)((n >>  0) & 0xFF);
    
    printf("A=%d,B=%d,C=%d,D=%d\n", A, B, C, D);
    
    return 0;
}