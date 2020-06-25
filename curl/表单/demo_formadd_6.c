#include <stdio.h>
#include <string.h>
 
#include <curl/curl.h>

/**
    要模拟的表单 demo_formadd_6.html
    简化如下：
    <form enctype="multipart/form-data" method="post" action="demo_formadd_6.php">  
    <input type="file" name="filename" />
    <input type="submit" name="submit "value="Submit me!"/>
    </form>  
    运行后调用脚本 demo_formadd_6.php
 */

/**
    如果出现 413 Request Entity Too Large 可能是服务端对客户端上传文件大小作了限制
    修改服务器配置文件重启试试, 这里将nginx服务器配置 
        client_max_body_size = 20m
    
    如果传输大文件,还需要对php.ini修改(参考: http://blog.sina.com.cn/s/blog_16698a9a40102yai4.html)
        upload_max_filesize = 20M
        post_max_size = 20M
        max_execcution_time = 120
        max_input_time = 300
    
    如果php出现其他问题,可以根据全局变量 $_FILES['XXX']['error'] 的值来判断
 */

 /**
    无论文本上传还是二进制上传,都是一种上传文件
    事实上,应该也可以用上传文件的方式上传二进制文件,反之亦然
 */
 
size_t read_callback(char *buffer, size_t size, size_t nitems, void *instream)
{
    fprintf(stdout, "get here:  size * nitems = %lu\n", size * nitems);
    return fread(buffer, size, nitems, instream);
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    
    CURL *curl;
    CURLcode res;
    struct curl_httppost *formpost=NULL;
    struct curl_httppost *lastptr=NULL;
    
    /** 6.上传大的二进制文件 */
    FILE *pf = fopen("./big.tar.gz", "rb");
    fseek(pf, 0, SEEK_END);
    long binsize = ftell(pf);
    fseek(pf, 0, SEEK_SET);
    curl_formadd(&formpost, &lastptr,
                CURLFORM_COPYNAME, "filename",
                CURLFORM_STREAM, pf,
                CURLFORM_CONTENTSLENGTH, binsize,
                CURLFORM_FILENAME, "server-big.tar.gz",     //在服务器上叫什么
                CURLFORM_CONTENTTYPE, "application/octet-stream",
                CURLFORM_END);
    /**
        如果 CURLFORM_CONTENTTYPE 在这里不指定，则php脚本无法判断文件类型，
        这说明php的能力有限，可能会判断出来，也可能不会
     */
                
    /** 提交动作 */ 
    curl_formadd(&formpost, &lastptr,
               CURLFORM_COPYNAME, "submit",
               CURLFORM_COPYCONTENTS, "Submit me!",
               CURLFORM_END);
    curl = curl_easy_init();
    
    curl_global_init(CURL_GLOBAL_ALL);
    if(curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.10.137/demo_formadd_6.php");
        curl_easy_setopt(curl, CURLOPT_POST, 1L);       //应该可以不用设
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);

        res = curl_easy_perform(curl);
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        else
            fprintf(stdout, "perform OK!\n");

        curl_easy_cleanup(curl);
        curl_formfree(formpost);
    }
    curl_global_cleanup();
    fclose(pf);
    return 0;
}
