#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

/**
    功能：解析命令行参数
    getopt函数：
        int getopt(int argc, char * const argv[], const char *optstring);
            1.argv由选项字符和选项参数构成，选项字符后可以不跟选项参数，但选项参数前必须有对应的选项字符；
            1+.命令自身、选项字符和选项参数，三者都是argv数组元素；
            2.每个选项字符之前有一个 '-'，但这只是可选项（不太了解）；
            3.要使用的全局变量 optopt optind optarg opterr;
                optopt  整型，用于记录返回的选项字符
                optind  整型，记录argv中即将处理的下个数组元素的下标，初始化值为1，可进行人工重置
                optarg  字符串，记录选项字符后对应的选项参数
                opterr  整型，记录错误
            4.getopt找到一个选项字符时，会返回此字符，且系统会将optind根据情况进行更新；
                示例 对命令行使用getopt进行循环扫描 ./prog -n -t 3600 -d 
                开始调用getopt扫描 optind=1，指向选项字符 -n，返回字符 'n', optind=2;
                再次调用getopt扫描 optind=2, 指向选项字符 -t, 返回字符 't', optarg="3600", optind=4;
                再次调用getopt扫描 optind=4, 指向选项字符 -d, 返回字符 'd', optind=5;
                再次调用getopt扫描，已经无选项字符，返回-1，扫描结束
            5.optstring 选项字符串
                示例 nt:d::
                字符后紧跟一个 : ,表示该选项字符后必须跟一个选项参数 如t 命令行表示为 -t 3600
                字符后紧跟两个 : ,表示该选项字符后可以跟一个选项参数 如d 但optarg并不存储此选项参数
                字符后不跟分号，如n
            6.如果扫描到无法识别的选项字符(即optstring不存在的字符)，optopt会返回 '?'，opterr记录非0
    getopt_long函数：
        省略
 */
// #define  DEBGU_GETOPT
int main(int argc, char *argv[])
{
    int flags, opt;
    int nsecs, tfnd;
    nsecs = 0;
    tfnd = 0;
    flags = 0;
    while ((opt = getopt(argc, argv, "nt:d::")) != -1)
    {
#ifdef DEBGU_GETOPT
        printf("%d opt=%c, optarg=%s, optind=%d, argv[%d]=%s\n", __LINE__, opt, optarg, optind, optind, argv[optind]);
#endif
        switch(opt)
        {
            case 'n':
                flags = 1;
                break;
            case 't':
                nsecs = atoi(optarg);
                tfnd = 1;
                break;
            default: /* '?' */
                fprintf(stderr, "Usage: %s [-t nsecs] [-n] name\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    printf("flags = %d; nsecs=%d; tfnd = %d; optind=%d\n", flags, nsecs, tfnd, optind);
    
    if (optind >= argc)
    {
        fprintf(stderr, "Expected argument after options\n");
        exit(EXIT_FAILURE);
    }
    printf("name argument = %s\n", argv[optind]);
    
    exit(EXIT_SUCCESS);
}