#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/**
 *  sscanf 
 *   将待解析字符串中的内容按照指定格式解析入相应变量
 *   返回值: 解析成功的数量
 */

int main()
{
    int ret = -1;
    
    {
        const char *str = "192.168.1.5:8080";
        unsigned int a, b, c, d, port = 0;
        int len = 0;
        
        printf("strlen(ipV4 string) = %lu\n", strlen(str));
        // len 返回待解析字符串的数量
        ret = sscanf(str, "%u.%u.%u.%u:%u%n", &a, &b, &c, &d, &port, &len);
        if (ret != 5) {
            printf("sscanf error(%d)\n", ret);
        }
        printf("a = %d, b = %d, c = %d, d = %d, port = %d, len = %d\n", a, b, c, d, port, len);
        // sa->sin.sin_addr.s_addr = htonl(((uint32_t) a << 24) | ((uint32_t) b << 16) | c << 8 | d);
        // sa->sin.sin_port = htons((uint16_t) port);
    }
    
    {
        const char *str = "[3ffe:2a00:100:7031::1]:8080";
        char buf[100] = { 0 };
        unsigned port = 0;
        int len = 0;
        
        printf("strlen(ipV6 string) = %lu\n", strlen(str));
        // 匹配字符，直到出现 ']' 为止。将匹配到的字符写入 buf 中，且最多写入 99 个字符
        ret = sscanf(str, "[%99[^]]]:%u%n", buf, &port, &len);
        if (ret != 2) {
            printf("sscanf error(%d)\n", ret);
        }
        printf("buf = %s, port = %d, len = %d\n", buf, port, len);
        // inet_pton(AF_INET6, buf, &sa->sin6.sin6_addr)
        // sa->sin.sin_port = htons((uint16_t) port);
    }
    
    {
        int n;
        
        char *p = NULL;
        char buf[1024];
        const char *str = "hello world!\n41424344";
        
        //遇空格结束
        n = sscanf(str, "%s", buf);
        printf("%d %d %s\n", __LINE__, n, buf);
        //遇行结束
        n = sscanf(str, "%[^\n]", buf);
        printf("%d %d %s\n", __LINE__, n, buf);
        //匹配字符直到不匹配
        n = sscanf(str, "%[a-n]", buf);
        printf("%d %d %s\n", __LINE__, n, buf);
        //匹配字符直到不匹配，并动态分配存储
        n = sscanf(str, "%m[a-n]", &p);
        printf("%d %d %s\n", __LINE__, n, p);
        free(p);
        
        printf("\n");
        uint32_t ui32;
        int64_t i64;
        int i = 0;
        const char *strdec = "414243";
        //截取前4个作为32位无符号整型 %n$
        n = sscanf(strdec, "%04u", &ui32);
        printf("%d %d %u\n", __LINE__, n, ui32);
        //读取作为64位有符号整型
        n = sscanf(strdec, "%ld", &i64);
        printf("%d %d %ld\n", __LINE__, n, i64);
        //以十六进制读取前两位 %n$
        n = sscanf(strdec, "%02x", &i);
        printf("%d %d %d\n", __LINE__, n, i);
        //以十交进制读取前两位（另一种方法）
        n = sscanf(strdec, "%ld", &i64);
        printf("%d %d %ld\n", __LINE__, n, i64);
        
        printf("\n");
        //读取hex数串，%i将以0x打头的解析为十六进制数
        const char *strhex = "0x414243";    //dec 4276803
        n = sscanf(strhex, "%i", &i);
        printf("%d %d %d\n", __LINE__, n, i);
        //读取ocx数串，%i将以0打头的解析为八进制数
        const char *stroct = "0414243";     //dec 137379
        n = sscanf(stroct, "%i", &i);
        printf("%d %d %d\n", __LINE__, n, i);
        
        printf("\n");
        //读取第一个字符串，忽略之后的字符串，直到读取一个整型
        n = sscanf(str, "%s %*s %d", buf, &i);
        printf("%d %d %s %d\n", __LINE__, n, buf, i);
    }
    
    
    return ret;
}
