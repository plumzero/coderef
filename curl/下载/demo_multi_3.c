#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <curl/multi.h>

// #define SUPPORT_SSL 1

#define MULTI_PERFORM_HANG_TIMEOUT 60 * 1000
#define CNT 5

char errbuf[CURL_ERROR_SIZE];
long verbose;

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

static const char *urls_default[] = {
    "https://192.168.10.130:7443/debian/ls-lR.gz",
    "https://192.168.10.130:7443/debian/project/ftpsync/ftpsync-current.tar.gz",
    "https://192.168.10.130:7443/debian/README.CD-manufacture",
    "https://192.168.10.130:7443/debian/indices/md5sums.gz",
    "https://192.168.10.130:7443/debian/indices/Uploaders"
};

static size_t cb(char *d, size_t n, size_t l, void *p)
{
  return fwrite(d, n, l, (FILE*)p);
}

static struct timeval tvnow(void)
{
    struct timeval now;
    now.tv_sec = (long)time(NULL);
    now.tv_usec = 0;

    return now;
}

static long tvdiff(struct timeval newer, struct timeval older)
{
    return (newer.tv_sec - older.tv_sec) * 1000 + (newer.tv_usec - older.tv_usec) / 1000;
}

static void init(CURLM *cm, const char *url)
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

    curl_easy_setopt(eh, CURLOPT_TIMEOUT, 300L);
    curl_easy_setopt(eh, CURLOPT_CONNECTTIMEOUT, 10L);
    
    curl_easy_setopt(eh, CURLOPT_HEADER, 0L);
    curl_easy_setopt(eh, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(eh, CURLOPT_ERRORBUFFER, errbuf);
    curl_easy_setopt(eh, CURLOPT_PRIVATE, url);
        
    if (verbose)
        curl_easy_setopt(eh, CURLOPT_VERBOSE, 1L);
    
    curl_multi_add_handle(cm, eh);
}

static void check_curlm_code(CURLMcode rcm)
{
    if (rcm != CURLM_OK)
    {
        fprintf(stderr, "%s::%s %d failed (errorcode: %d) %s\n", __FILE__, __FUNCTION__, __LINE__, rcm, curl_multi_strerror(rcm));
        exit(1);
    }
}

static void easys_clear(CURLM* cm, const int still_running)
{   
    CURLMsg *msg;
    int msgs_left;
    CURL *easy;
    CURLcode res;
    
    while((msg = curl_multi_info_read(cm, &msgs_left)))
    {
        if(msg->msg == CURLMSG_DONE)
        {
            easy = msg->easy_handle;
            res = msg->data.result;
            curl_multi_remove_handle(cm, easy);
            curl_easy_cleanup(easy);
        }
        else if (still_running)
        {
            fprintf(stdout, "%s::%s %d select error\n", __FILE__, __FUNCTION__, __LINE__);
        }
    }
}

static void multi(CURLM* cm)
{
    struct timeval mp_start;
    mp_start = tvnow();
    int still_running = -1;
    int flag = 0;       /* 超时标志 1-超时中 */
    while (still_running) 
    {
        struct timeval timeout;
        fd_set fdread;
        fd_set fdwrite;
        fd_set fdexcep;
        int maxfd = -1;
        long curl_timeo = -1;

        FD_ZERO(&fdread);
        FD_ZERO(&fdwrite);
        FD_ZERO(&fdexcep);

        /* 备用超时值 */
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        /* 获取libcurl提供的超时，此超时用作select的超时 */
        curl_multi_timeout(cm, &curl_timeo);
        if (curl_timeo >= 0) 
        {
            timeout.tv_sec = curl_timeo / 1000;
            if (timeout.tv_sec > 1)
                timeout.tv_sec = 1;
            else
                timeout.tv_usec = (curl_timeo % 1000) * 1000;
        }

        /* 获取multi会话中的描述符集 */
        CURLMcode rcm = curl_multi_fdset(cm, &fdread, &fdwrite, &fdexcep, &maxfd);
        check_curlm_code(rcm);

        int rc;
        if (maxfd == -1) 
        {
            /** 
                select可能在做其他的事情，此时不必监控任何套接字描述符，休眠100ms。
                100ms之后，如果如果描述符还未有变化，阻塞在while
             */
            struct timeval wait = { 0, 100 * 1000 };
            rc = select(0, NULL, NULL, NULL, &wait);
        }
        else
        {
            rc = select(maxfd + 1, &fdread, NULL, NULL, &timeout);
        }

        if (flag == 1 && tvdiff(tvnow(), mp_start) > MULTI_PERFORM_HANG_TIMEOUT) 
        {
            fprintf(stderr, "ABORTING: Since it seems that we would have run forever.\n");
            break;
        }

        /* 超时也要perform，明确是select还是easy的问题 */
        switch(rc)
        {
            case -1:
                if (flag == 0)
                {
                    mp_start = tvnow();
                    flag = 1;
                }
                break;
            case 0:
                curl_multi_perform(cm, &still_running);
                if (flag == 0)
                {
                    mp_start = tvnow();
                    flag = 1;
                }
                break;  
            default:
                curl_multi_perform(cm, &still_running);
                flag = 0;
                break;
        }
    }
    
    easys_clear(cm, still_running);
}

int main(int argc, char *argv[])
{
    CURLM* cm;
    CURLMcode res;
    int i;
    const char* default_url = "https://192.168.10.130:7443/debian/ls-lR.gz";
    const char* urls[CNT];
    for (i = 0; i < CNT; i++)
        urls[i] = NULL;
    errbuf[0] = '\0';
    const char* type = "sm2", *keytype = NULL;
    verbose = 0;
    
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
            verbose = 1;
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

    cm = curl_multi_init();

    for (i = 0; i < url_counter; ++i) 
    {
        init(cm, urls[i]);
    }

    multi(cm);
    
    curl_multi_cleanup(cm);

    return 0;
}