#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <sys/epoll.h>
#include<curl/curl.h>

// #define SUPPORT_SSL 1

#define CNT 10

/**
    epoll 代替 libevent
    此函数的执行方式可以参考: demo_multi_libevent.c
    
    三种方法总结(建议优先使用顺序排序)：
        1.curl_multi_socket_action + 基于事件机制的触发回调 + curl_multi_assign
        2.curl_multi_wait + curl_multi_perform
        2.curl_multi_fdset + curl_multi_timeout + select + curl_multi_perform
 */
typedef struct _global_info 
{
    int epfd;
    CURLM *multi;
    int verbose;
} global_info;

typedef struct _multi_curl_sockinfo 
{
    CURL *cp;
    struct epoll_event ev;
} multi_curl_sockinfo;


char *urls_default[] = {
    "https://192.168.10.130:7443/debian/ls-lR.gz",
    "https://192.168.10.130:7443/debian/project/ftpsync/ftpsync-current.tar.gz",
    "https://192.168.10.130:7443/debian/README.CD-manufacture",
    "https://192.168.10.130:7443/debian/indices/md5sums.gz",
    "https://192.168.10.130:7443/debian/indices/Uploaders"
};

static int usage()
{
    printf(" TLS client/server both verify with callback.\n");
    printf("--url the https URL (like:https://192.168.10.130:7443)\n");
    printf("--five download five specified files together.\n");
    printf("--keytype like('sm2','rsa','secp256r1','secp384r1','secp521r1')\n");
    printf("--help print this message.\n");
    printf("-v Enable verbose.\n");
    exit(-1);
}

static size_t cb(char *d, size_t n, size_t l, void *p)
{
    return fwrite(d, n, l, (FILE*)p);
}

static int sock_cb (CURL *e, curl_socket_t s, int what, void *cbp, void *sockp)
{
    global_info * g = (global_info *) cbp;
    multi_curl_sockinfo  *fdp = (multi_curl_sockinfo *) sockp;
    
    if (what == CURL_POLL_REMOVE) 
    {
        fprintf(stdout, "The specified socket is no longer used by libcurl\n");
        if (fdp) 
        {
            free(fdp);
        }
        epoll_ctl(g->epfd, EPOLL_CTL_DEL, s, &(fdp->ev));
    } 
    else 
    {   
        if (!fdp)
        {
            fdp = (multi_curl_sockinfo *)malloc(sizeof(multi_curl_sockinfo));
            fdp->cp = e;
            (fdp->ev).events = (what & CURL_POLL_IN ? EPOLLIN : 0) | (what & CURL_POLL_OUT ? EPOLLOUT : 0) | (what & CURL_POLL_INOUT ? EPOLLIN | EPOLLOUT : 0);
            (fdp->ev).data.fd = s;
            epoll_ctl(g->epfd, EPOLL_CTL_ADD, s, &(fdp->ev));
            curl_multi_assign(g->multi, s, fdp);
        }
        else if (fdp)
        {
            (fdp->ev).events = (what & CURL_POLL_IN ? EPOLLIN : 0) | (what & CURL_POLL_OUT ? EPOLLOUT : 0) | (what & CURL_POLL_INOUT ? EPOLLIN | EPOLLOUT : 0);
            /** 
                分开来写，下面注释的地方
                本例中只会走第二个判断，猜测是因为本例只对读描述符感兴趣的原因，由可写转为可读
             */
            // if (what == CURL_POLL_OUT) 
            // {
                // (fdp->ev).events = EPOLLOUT;
            // }
            // else if (what == CURL_POLL_IN)
            // {
                // (fdp->ev).events = EPOLLIN;
            // }
            // else if (what == CURL_POLL_INOUT) 
            // {
                // (fdp->ev).events = EPOLLIN | EPOLLOUT;
            // }
        }
    }
    
    return 0;
}

static void init(global_info* g, const char *url)
{
    char tmp_chars[1024];
    strcpy(tmp_chars, url);
    char *file_name = strrchr(tmp_chars, 47) + 1;
    char dd_path[1024];
    strcpy(dd_path, "./");
    strcat(dd_path, file_name);

    FILE* pFile = fopen(dd_path, "wb");

    CURL *eh = curl_easy_init();
    curl_easy_setopt(eh, CURLOPT_URL, url);
    curl_easy_setopt(eh, CURLOPT_WRITEFUNCTION, cb);
    curl_easy_setopt(eh, CURLOPT_WRITEDATA, pFile);

#ifdef SUPPORT_SSL
    curl_easy_setopt(eh, CURLOPT_CAINFO, "./kpool/_sm2.ca.crt.pem");
    curl_easy_setopt(eh, CURLOPT_SSL_VERIFYSTATUS, 1L);
    curl_easy_setopt(eh, CURLOPT_SSL_VERIFYHOST, 2L);
    curl_easy_setopt(eh, CURLOPT_SSLCERTTYPE, "PEM");
    curl_easy_setopt(eh, CURLOPT_SSLCERT, "./kpool/_sm2.client.crt.pem");
    curl_easy_setopt(eh, CURLOPT_SSLKEYTYPE, "PEM");
    curl_easy_setopt(eh, CURLOPT_SSLKEY, "./kpool/_sm2.client.pvk.pem");
    curl_easy_setopt(eh, CURLOPT_SSL_VERIFYPEER, 1L);
#endif
    // curl_easy_setopt(eh, CURLOPT_TIMEOUT, 300L);
    // curl_easy_setopt(eh, CURLOPT_CONNECTTIMEOUT, 10L);

    curl_easy_setopt(eh, CURLOPT_HEADER, 0L);
    curl_easy_setopt(eh, CURLOPT_NOSIGNAL, 1L);
        
    if (g->verbose)
        curl_easy_setopt(eh, CURLOPT_VERBOSE, 1L);

    curl_multi_add_handle(g->multi, eh);
}

static void check_multi_info(global_info *g)
{
    CURLMsg *msg;
    int msgs_left;
    CURL *easy;
    char *eff_url;

    while((msg = curl_multi_info_read(g->multi, &msgs_left)))
    {
        if(msg->msg == CURLMSG_DONE)
        {
            easy = msg->easy_handle;
            CURLcode res = msg->data.result;
            curl_easy_getinfo(easy, CURLINFO_EFFECTIVE_URL, &eff_url);
            fprintf(stdout, "DOWNLOAD: %s OK\n", eff_url);
            curl_multi_remove_handle(g->multi, easy);
            curl_easy_cleanup(easy);
        }
    }
}

int main(int argc, char *argv[])
{
    int i;
    global_info g;
    memset(&g, 0, sizeof(global_info));
    const char* default_url = "https://192.168.10.130:7443/debian/ls-lR.gz";
    const char* urls[CNT];
    for (i = 0; i < CNT; i++)
        urls[i] = NULL;
    const char* type = "sm2", *keytype = NULL;

    argc--;
    argv++;
    
    int url_index = 0;
    while(argc > 0)
    {
        if (strcmp(*argv, "--url") == 0)
        {
            if (--argc < 1)
                break;
            urls[url_index] = *(++argv);
            url_index++;
        }
        else if(strcmp(*argv, "--keytype") == 0)
        {
            if (--argc < 1)
                break;
            keytype = *(++argv);
            url_index = 0;
        }
        else if (strcmp(*argv, "--help") == 0)
        {
            usage();
            url_index = 0;
        }
        else if (strcmp(*argv, "-v") == 0)
        {
            g.verbose = 1;
            url_index = 0;
        }
        else if (strcmp(*argv, "--five") == 0)
        {
            /* 单独的测试，不要加--url */
            for (i = 0; i < (int)(sizeof(urls_default)/sizeof(urls_default[0])) && i < CNT; i++)
                urls[i] = urls_default[i];
        }
        else if (url_index)
        {
            if (url_index == CNT)
            {
                argc--;
                argv++;
                continue;
            }
            urls[url_index] = *argv;
            url_index++;
        }
        else
            break;
        argc--;
        argv++;
    }
    
    if (!urls[0])
        urls[0] = default_url;
    
    int url_counter = 0;
    for (i = 0; i < CNT; i++)
    {
        if (urls[i])
            url_counter++;
    }
    
    if (keytype)
    {
        if (strcmp(keytype, "rsa") == 0 || strcmp(keytype, "sm2") == 0)
            type = keytype;
        else if(strcmp(keytype, "secp256r1") == 0
                || strcmp(keytype, "secp384r1") == 0
                || strcmp(keytype, "secp521r1") == 0)
            type = "ecc";
        else
            type = "none";
    }
    
    curl_global_init(CURL_GLOBAL_ALL);
    g.epfd = epoll_create(CNT);
    g.multi = curl_multi_init();
    curl_multi_setopt(g.multi, CURLMOPT_SOCKETFUNCTION, sock_cb);
    curl_multi_setopt(g.multi, CURLMOPT_SOCKETDATA, &g);

    for (i = 0; i < url_counter; i++)
    {
        init(&g, urls[i]);
    }

    int running_count;
    struct epoll_event events[CNT];
    curl_multi_socket_action(g.multi, CURL_SOCKET_TIMEOUT, 0, &running_count);
    if (running_count)
    {
        do {
            /** 
                这里手动设置超时为500ms
                注意：
                    这里不能调用 curl_multi_timeout 函数来获取系统提供的超时赋给 epoll_wait
                    可以在curl_multi_setopt中设置 CURLMOPT_TIMERFUNCTION 选项来获得同样的期望结果
                
                额外地，curl_multi_timeout 常与 select 合用，但无论如何，最好对 curl_mutl_timout 的返回值作一个判断
        
             */
            int nfds = epoll_wait(g.epfd, events, 10, 500); 
            if(nfds > 0)
            {
                for (i = 0; i < nfds; i++) 
                {
                    /**
                        epoll_wait后，检测到描述符动作有变化，立即调用 curl_multi_socket_action
                        本例中只会走第二个判断
                            描述符状态虽然为可写，但水位已达到mark，现在调用 curl_multi_socket_action 将其转为可读。此时描述符变化，立即调用 sock_cb
                     */
                    if (events[i].events & EPOLLIN)
                    {
                        curl_multi_socket_action(g.multi, events[i].data.fd, CURL_CSELECT_IN, &running_count);
                    } 
                    else if (events[i].events & EPOLLOUT)
                    {
                        curl_multi_socket_action(g.multi, events[i].data.fd, CURL_CSELECT_OUT, &running_count);
                    }
                    check_multi_info(&g);
                }
            }
            else
            {
                curl_multi_socket_action(g.multi, CURL_SOCKET_TIMEOUT, 0, &running_count);
                if (running_count)
                {
                    fprintf(stderr, "although there are maybe some easy handle running, but we can't find any fd changed, \
                                now we give you a timeout, if it expires but still unchanged ,some error may occured\n");
                    break;  /** error occured! */
                }
            }
        } while (running_count);
    }

    curl_global_cleanup();
    return 0;
}