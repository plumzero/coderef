#include <errno.h>
#include <stdlib.h>
#include <string.h>
#ifndef WIN32
#  include <unistd.h>
#endif
#include <curl/multi.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <sys/io.h>

#define THREAD_NUM 30

// #define SUPPORT_SSL 1

typedef struct _interval
{
    size_t begin_iter;
    size_t end_iter;
    int use;
} INTERV;

pthread_t threads[THREAD_NUM];
INTERV g_interval[THREAD_NUM];
FILE* g_pFile;

pthread_mutex_t g_mutex;
pthread_mutex_t g_mutex_index;

static long get_download_file_length(const char *url)
{
    double file_length;
    int res;
    CURL *curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_HEADER, 1L);
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    
#ifdef SUPPORT_SSL
    curl_easy_setopt(curl, CURLOPT_CAINFO, "./kpool/_sm2.ca.crt.pem");
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYSTATUS, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
    curl_easy_setopt(curl, CURLOPT_SSLCERTTYPE, "PEM");
    curl_easy_setopt(curl, CURLOPT_SSLCERT, "./kpool/_sm2.client.crt.pem");
    curl_easy_setopt(curl, CURLOPT_SSLKEYTYPE, "PEM");
    curl_easy_setopt(curl, CURLOPT_SSLKEY, "./kpool/_sm2.client.pvk.pem");
#endif
    
    if ((res = curl_easy_perform(curl)) != CURLE_OK)
    {
        fprintf(stderr, "%s::%s %d failed: %s\n", __FILE__, __FUNCTION__, __LINE__, curl_easy_strerror(res));
        file_length = -1;
    }
    else
    {
        if ((res = curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &file_length)) != CURLE_OK)
        {
            fprintf(stderr, "%s::%s %d failed: %s\n", __FILE__, __FUNCTION__, __LINE__, curl_easy_strerror(res));
            file_length = -1;
        }
    }
    curl_easy_cleanup(curl);
    return file_length;
}

//CURLOPT_WRITEFUNCTION项回调函数
static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    INTERV *p_interv = (INTERV*)userdata;
    size_t written;
    //多线程写同一个文件要加锁
    pthread_mutex_lock(&g_mutex);
    if (size * nmemb <= p_interv->end_iter - p_interv->begin_iter + 1)
    {
        fseek(g_pFile, p_interv->begin_iter, SEEK_SET);
        written = fwrite(ptr, size, nmemb, g_pFile);
        p_interv->begin_iter += size * nmemb;
    }
    else
    {
        fseek(g_pFile, p_interv->begin_iter, SEEK_SET);
        written = fwrite(ptr, size, p_interv->end_iter - p_interv->begin_iter + 1, g_pFile);
    }
    pthread_mutex_unlock(&g_mutex);
    
    return written; 
}

static int download_directly(const char *url, INTERV *p_interv)
{
    int res;
    char range[200];
    sprintf(range, "%ld-%ld", p_interv->begin_iter, p_interv->end_iter);
    
    CURL *curl = curl_easy_init();
    
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, p_interv);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_RANGE, range);
    
#ifdef SUPPORT_SSL
    curl_easy_setopt(curl, CURLOPT_CAINFO, "./kpool/_sm2.ca.crt.pem");
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYSTATUS, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
    curl_easy_setopt(curl, CURLOPT_SSLCERTTYPE, "PEM");
    curl_easy_setopt(curl, CURLOPT_SSLCERT, "./kpool/sm2.client.crt.pem");
    curl_easy_setopt(curl, CURLOPT_SSLKEYTYPE, "PEM");
    curl_easy_setopt(curl, CURLOPT_SSLKEY, "./kpool/sm2.client.pvk.pem");
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
#endif
    
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 300L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
    
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(curl, CURLOPT_PROXY, "");
    
    if ((res = curl_easy_perform(curl)) != CURLE_OK)
    {
        fprintf(stderr, "%s::%s %d failed: %s\n", __FILE__, __FUNCTION__, __LINE__, curl_easy_strerror(res));
        return -1;
    }
    
    curl_easy_cleanup(curl);
    return 0;
}

void slice_file(const char *url)
{
    long file_length = get_download_file_length(url);
    long range = file_length / (THREAD_NUM - 1);
    long size = 0;
    for (int i = 0; i < THREAD_NUM; i++)
    {
        g_interval[i].begin_iter = size;
        g_interval[i].end_iter = (size + range <= file_length ? size + range - 1 : file_length - 1);
        size += range;
    }
}

void *thread_func(void *args)
{
    int i;
    const char *url = args;
    
    //加锁
    int index = 0;
    pthread_mutex_lock(&g_mutex_index);
    for (i = 0; i < THREAD_NUM; i++)
    {
        if (!g_interval[i].use)
        {
            index = i;
            break;
        }
    }
    g_interval[index].use = 1;
    pthread_mutex_unlock(&g_mutex_index);
    
    download_directly(url, &g_interval[index]);
    
    return NULL;
}

int create_threads(char* url)
{
    int ret;
    for (int i = 0; i < THREAD_NUM; i++)
    {
        ret = pthread_create(&threads[i], NULL, thread_func, url);
        if (ret)
        {
            fprintf(stderr, "%s::%s %d failed\n", __FILE__, __FUNCTION__, __LINE__);
            return -1;
        }
    }
    return 0;
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    
    char url[] = { "https://192.168.10.130:7443/debian/ls-lR.gz" };
    char tmp_chars[1024];
    strcpy(tmp_chars, url);
    char *file_name = strrchr(tmp_chars, 47) + 1;
    char dd_path[1024];
    strcpy(dd_path, "./");
    strcat(dd_path, file_name);
    g_pFile = fopen(dd_path, "wb");
    
    curl_global_init(CURL_GLOBAL_ALL);
    
    slice_file(url);
    
    create_threads(url);
    
    for (int i = 0; i < THREAD_NUM; i++)
        pthread_join(threads[i], NULL);
    
    curl_global_cleanup();
    
    return 0;
}