#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <sys/poll.h>
#include <curl/curl.h>
#include <event2/event.h>
#include <errno.h>

//全局
typedef struct _GlobalInfo
{
    struct event_base *evbase;
    struct event *timer_event;
    CURLM *multi;
    int still_running;
} GlobalInfo;

//连接的easy信息
typedef struct _ConnInfo
{
    CURL *easy;
    char *url;
    GlobalInfo *global;
    char error[CURL_ERROR_SIZE];
    FILE *pFile;
} ConnInfo;

//socket状态信息
typedef struct _SockInfo
{
    curl_socket_t sockfd;
    CURL *easy;
    int action;
    long timeout;
    struct event *ev;
    int evset;
    GlobalInfo *global;
} SockInfo;

static const char *urls[] = {
    "http://nginx.org/download/nginx-1.14.0.tar.gz",
    "http://nginx.org/download/nginx-1.12.2.tar.gz",
    "http://nginx.org/download/nginx-1.10.3.tar.gz",
    "http://nginx.org/download/nginx-1.8.1.tar.gz",
    "http://nginx.org/download/nginx-1.6.3.tar.gz"
};

/**
    描述符没有变化时会调用，timeout_ms 是libcurl自己指定的超时
    这里可能会在三种情况下调用（发生2情况时极可能出错，3情况一般会在 CURLMOPT_SOCKETFUNCTION 设置的回调中进行处理）
        1.multi会话创建完成后，初始化MULTI_SOCKET
        2.multi会话中有easy handle在运行，所监听的描述符没有动作变化（调大超时重新进行此回调看看）
        3.multi会话中无easy_handle运行，描述符不再变化，调用此超时进行处理
 */
static int multi_timer_cb(CURLM *multi, long timeout_ms, GlobalInfo *g)
{
    printf("************************line = %d\n", __LINE__);
    struct timeval timeout;

    timeout.tv_sec = timeout_ms / 1000;
    timeout.tv_usec = (timeout_ms % 1000) * 1000;

    evtimer_add(g->timer_event, &timeout);  //将设置好的超时事件加入调度中
    
    return 0;
}

static void check_curlm_code(CURLMcode rc)
{
    if (rc != CURLM_OK)
    {
        fprintf(stderr, "libcurl multi failed (%d) %s\n", rc, curl_multi_strerror(rc));
        exit(1);
    }
}

/** 检测传输是否完成 */
static void check_multi_info(GlobalInfo *g)
{
    char *eff_url;
    CURLMsg *msg;
    int msgs_left;
    ConnInfo *conn;
    CURL *easy;
    CURLcode res;

    /**
        传送完成或发生错误时，会产生一个存放信息体的msg，其他情况返回NULL
        有时虽msgs_left不为0，但msg是NULL
    */
    while((msg = curl_multi_info_read(g->multi, &msgs_left)))
    {
        if(msg->msg == CURLMSG_DONE) /** 只有为 CURLMSG_DONE 时，才表示 transfer success */
        {
            easy = msg->easy_handle;
            res = msg->data.result;
            curl_easy_getinfo(easy, CURLINFO_PRIVATE, &conn);
            curl_easy_getinfo(easy, CURLINFO_EFFECTIVE_URL, &eff_url);
            fprintf(stdout, "DONE: %s => (%d) %s\n", eff_url, res, conn->error);
            curl_multi_remove_handle(g->multi, easy);
            free(conn->url);
            curl_easy_cleanup(easy);
            free(conn);
        }
        //else if (一直超时 && g->still_running) { ... }
    }
}

/** 一旦描述符有变化，就立刻调用 */
static void event_cb(int fd, short kind, void *userp)
{
    GlobalInfo *g = (GlobalInfo*) userp;
    CURLMcode rc;

    int action = (kind & EV_READ ? CURL_CSELECT_IN : 0) | (kind & EV_WRITE ? CURL_CSELECT_OUT : 0);

    /** 描述符有变化，开启对应该描述符的easy handle所设置选项及回调，perform */
    rc = curl_multi_socket_action(g->multi, fd, action, &g->still_running);
    check_curlm_code(rc);

    /** 每次perform后（这里是读操作完成后），判断transfer是否完成 */
    check_multi_info(g);    
    if(g->still_running <= 0) 
    {
        fprintf(stdout, "last transfer done, kill timeout\n");
        /** 下面的判断应该也可以在 CURLMOPT_TIMERFUNCTION 的回调中进行 */
        if(evtimer_pending(g->timer_event, NULL))
        {
            /** 将超时事件从调度中删除 */
            evtimer_del(g->timer_event);
        }
    }
}

static void timer_cb(int fd, short kind, void *userp)
{
    GlobalInfo *g = (GlobalInfo *)userp;
    CURLMcode rc;
    rc = curl_multi_socket_action(g->multi, CURL_SOCKET_TIMEOUT, 0, &g->still_running);
    check_curlm_code(rc);
    //检测 multi 会话是否结束（这里是传输是否完成）
    check_multi_info(g);
}

//记录socket状态
static void setsock(SockInfo *f, curl_socket_t s, CURL *e, int act, GlobalInfo *g)
{   
    int kind = (act & CURL_POLL_IN ? EV_READ : 0) | (act & CURL_POLL_OUT ? EV_WRITE : 0) | EV_PERSIST;
    
    f->sockfd = s;
    f->action = act;
    f->easy = e;
    if(f->evset)
        event_free(f->ev);
    f->ev = event_new(g->evbase, f->sockfd, kind, event_cb, g); //将描述符添加到调度中
    f->evset = 1;
    event_add(f->ev, NULL);
}


//监控的描述符属性一旦有变化，会立即调用这个
static int sock_cb(CURL *e, curl_socket_t s, int what, void *cbp, void *sockp)
{
    GlobalInfo *g = (GlobalInfo*) cbp;
    SockInfo *fdp = (SockInfo*) sockp;  //记录此次socket状态信息，创建后便保存这个指针

    //使用switch判断就不行（未知）
    if (what == CURL_POLL_REMOVE)
    {
        fprintf(stdout, "The specified socket is no longer used by libcurl\n");
        //必须要清除event_free，否则就算完成也一直调度下去，所以要保存socket状态信息
        if(fdp) 
        {
            if(fdp->evset)
                event_free(fdp->ev);
            free(fdp);
        }
    }
    else if (what == CURL_POLL_IN || what == CURL_POLL_OUT || what == CURL_POLL_INOUT)
    {
        if (!fdp)
        {
            fdp = malloc(sizeof(SockInfo));
            memset(fdp, 0, sizeof(SockInfo));
            fdp->global = g;
            setsock(fdp, s, e, what, g);
            curl_multi_assign(g->multi, s, fdp);    //此次状态信息保存到multi中
        }
        else
        {
            setsock(fdp, s, e, what, g);
        }   
    }
    
    return 0;
}

//CURLOPT_WRITEFUNCTION回调
static size_t write_cb(void *ptr, size_t size, size_t nmemb, void *data)
{
    ConnInfo *conn = (ConnInfo*) data;
    return fwrite(ptr, size, nmemb, conn->pFile);
}

//创建easy，加入multi
static void new_conn(char *url, GlobalInfo *g)
{
    ConnInfo *conn;
    CURLMcode rc;

    conn = malloc(sizeof(ConnInfo));
    memset(conn, 0, sizeof(ConnInfo));
    conn->error[0]='\0';
    
    char tmp_chars[1024];
    strcpy(tmp_chars, url);
    char* file_name = strrchr(tmp_chars, 47) + 1;
    char dd_path[1024];
    strcpy(dd_path, "./");
    strcat(dd_path, file_name);
    
    conn->pFile = fopen(dd_path, "wb");
    
    conn->easy = curl_easy_init();
    if(!conn->easy) 
    {
        exit(2);
    }
    conn->global = g;
    conn->url = strdup(url);
    curl_easy_setopt(conn->easy, CURLOPT_URL, conn->url);
    curl_easy_setopt(conn->easy, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(conn->easy, CURLOPT_WRITEDATA, conn);
    // curl_easy_setopt(conn->easy, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(conn->easy, CURLOPT_ERRORBUFFER, conn->error);
    curl_easy_setopt(conn->easy, CURLOPT_PRIVATE, conn);
    //多文件件下载进度较难展示 for me
    // curl_easy_setopt(conn->easy, CURLOPT_XFERINFOFUNCTION, prog_cb);
    // curl_easy_setopt(conn->easy, CURLOPT_XFERINFODATA, conn);
    // curl_easy_setopt(conn->easy, CURLOPT_NOPROGRESS, 0L);
    rc = curl_multi_add_handle(g->multi, conn->easy);
    check_curlm_code(rc);
}

//根据下载链接数创建easy
static void create_easys(GlobalInfo *g)
{
    for (int i = 0; i < sizeof(urls)/sizeof(urls[i]); i++)
    {
        new_conn(urls[i], g);
    }
}

/*
    1.只有一条线程，不涉及线程安全的问题
    2.采用libevent事件触发，socket属性有变（新建或读写之间转换），就为该描述符创建一个事件
      监听，并添加到调度循环中去。一旦描述符读写水位达到，调用事件回调，此回调中通过curl_
      multi_socket_action检查在easy选项中设置的回调选项，完成读或写。每完成一次读写，检查
      一次multi会话是否完毕
    3.如果socket属性长期不变，调用超时，超时中调用curl_multi_socket_action，检测会话是否结
      束。如果multi会话未结束，且一直超时，应在check_multi_info作处理（这里未作处理，不知）
    
    关键函数
        curl_multi_socket_action
 */
int main(int argc, char **argv)
{
    GlobalInfo g;
    memset(&g, 0, sizeof(GlobalInfo));
    
    g.evbase = event_base_new();
    g.multi = curl_multi_init();
    g.timer_event = evtimer_new(g.evbase, timer_cb, &g);

    //设置multi选项
    curl_multi_setopt(g.multi, CURLMOPT_SOCKETFUNCTION, sock_cb);
    curl_multi_setopt(g.multi, CURLMOPT_SOCKETDATA, &g);
    curl_multi_setopt(g.multi, CURLMOPT_TIMERFUNCTION, multi_timer_cb);
    curl_multi_setopt(g.multi, CURLMOPT_TIMERDATA, &g);

    create_easys(&g);
    
    //调度循环
    event_base_dispatch(g.evbase);

    event_free(g.timer_event);
    event_base_free(g.evbase);
    curl_multi_cleanup(g.multi);
    return 0;
}
