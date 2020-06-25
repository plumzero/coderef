#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <curl/multi.h>

// #define SUPPORT_SSL 1

#define MAX_WAIT_MSECS 10*1000 /* Wait max. 10 seconds */

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
    /** 
        单向认证时可以去掉下面四行
        不去掉也不影响 
     */
    curl_easy_setopt(eh, CURLOPT_SSLCERT, "./kpool/_sm2.client.crt.pem");
    curl_easy_setopt(eh, CURLOPT_SSLKEYTYPE, "PEM");
    curl_easy_setopt(eh, CURLOPT_SSLKEY, "./kpool/_sm2.client.pvk.pem");
    curl_easy_setopt(eh, CURLOPT_SSL_VERIFYPEER, 1L);
#endif    

    /** 
        下载多个文件时 CURLOPT_TIMEOUT 对应参数不要设置太短，即使文件很小
        下载单个文件时 CURLOPT_TIMEOUT 对应参数大小不作要求，即使文件很大
     */
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

static void multi(CURLM* cm)
{   
    /**
        如果有 multi 会话中有 easy 存在，执行与 easy 相关的回调函数
        为避免阻塞，为 multi 会话设定一个超时
     */
    int still_running = -1;
    while (still_running)
    {
        curl_multi_perform(cm, &still_running);
        
        int numfds = 0;
        CURLMcode rcm = curl_multi_wait(cm, NULL, 0, MAX_WAIT_MSECS, &numfds);
        check_curlm_code(rcm);
    }

    /**
        没有 easy 在 multi 会话中执行，说明 easy 已经执行完成或出现错误
        如果已经执行完成，处理 easy 资源
     */
    CURLMsg *msg=NULL;
    int msgs_left=0;
    while ((msg = curl_multi_info_read(cm, &msgs_left))) 
    {
        if (msg->msg == CURLMSG_DONE)
        {
            CURL* eh = msg->easy_handle;
            CURLcode rce = msg->data.result;
            if(rce != CURLE_OK) 
            {
                fprintf(stderr, "%s::%s %d failed: %s\n", __FILE__, __FUNCTION__, __LINE__, curl_easy_strerror(rce));
                continue;
            }
            
            int http_status_code = 1;
            char* szUrl = NULL;

            curl_easy_getinfo(eh, CURLINFO_RESPONSE_CODE, &http_status_code);
            curl_easy_getinfo(eh, CURLINFO_PRIVATE, &szUrl);

            if(http_status_code == 200) 
            {
                printf("200 OK for %s\n", szUrl);
            } 
            else 
            {
                fprintf(stderr, "GET of %s returned http status code %d\n", szUrl, http_status_code);
            }

            curl_multi_remove_handle(cm, eh);
            curl_easy_cleanup(eh);
        }
        else 
        {
            fprintf(stderr, "error: after curl_multi_info_read(), CURLMsg=%d\n", msg->msg);
        }
    }   
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