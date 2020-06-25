#include <stdio.h>
#include <string.h>
 
#include <curl/curl.h>

/**
    要模拟的表单 demo_formadd_5.html
    简化如下：
    <form enctype="multipart/form-data" method="post" action="demo_formadd_5.php">  
    <input type="file" name="filename" />
    <input type="submit" name="submit "value="Submit me!"/>
    </form>  
    运行后调用脚本 demo_formadd_5.php
*/

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    
    CURL *curl;
    CURLcode res;
    struct curl_httppost *formpost=NULL;
    struct curl_httppost *lastptr=NULL;

    /** 5.上传二进制文件（小型文件，如图片） */
    FILE *pf = fopen("./pic.png", "rb");
    fseek(pf, 0, SEEK_END);
    long binsize = ftell(pf);
    fseek(pf, 0, SEEK_SET);
    char buf[8192];     //这里二进制文件大小不能超过8192字节，否则报段错误
    fread(buf, 1, (size_t)binsize, pf);
    curl_formadd(&formpost, &lastptr,
                CURLFORM_COPYNAME, "filename",
                CURLFORM_BUFFER, "new.png",     //$_FILES['filename']['name'] = "bin_file" ，不同于CURLFORM_FILENAME，这个一定要设
                CURLFORM_BUFFERPTR, buf,
                CURLFORM_BUFFERLENGTH, binsize,
                CURLFORM_END);

    /** 提交动作 */ 
    curl_formadd(&formpost, &lastptr,
               CURLFORM_COPYNAME, "submit",
               CURLFORM_COPYCONTENTS, "Submit me!",
               CURLFORM_END);
    curl = curl_easy_init();
    
    curl_global_init(CURL_GLOBAL_ALL);
    if(curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.10.137/demo_formadd_5.php");
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

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
