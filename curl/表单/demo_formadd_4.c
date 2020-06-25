#include <stdio.h>
#include <string.h>
 
#include <curl/curl.h>

/**
    要模拟的表单 demo_formadd_4.html
    简化如下：
    <input type="file" name="filename"/>
    <input type="submit" name="submit "value="Submit me!"/>
    </form>  
    运行后调用脚本 demo_formadd_4.php
*/

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    
    CURL *curl;
    CURLcode res;
    struct curl_httppost *formpost=NULL;
    struct curl_httppost *lastptr=NULL;

    /** 4.上传文本文件 */
    curl_formadd(&formpost, &lastptr,
                CURLFORM_COPYNAME, "filename",
                // CURLFORM_FILENAME, "nickname",       //php脚本中 $_FILES['filename']['name'] = "nickname" 而不是 upload.txt，一般不设
                CURLFORM_FILE, "./upload.txt",
                CURLFORM_CONTENTTYPE, "text/plain",     //upload.txt的文件类型，这里如果你不指定的话，系统会自己检测
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
        curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.10.137/dir/5.php");
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
    return 0;
}
