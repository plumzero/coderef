#include <stdio.h>
#include <string.h>
 
#include <curl/curl.h>

/**
    要模拟的表单 demo_formadd_1.html
    简化如下：
    <input type="text" name="username"/>
    <input type="text" name="userage"/>
    <input type="submit" name="submit "value="Submit me!"/>
    运行后调用脚本 demo_formadd_1.php
*/

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    
    CURL *curl;
    CURLcode res;
    struct curl_httppost *formpost=NULL;
    struct curl_httppost *lastptr=NULL;

    /** 1.上传字符串 */
    curl_formadd(&formpost, &lastptr,
                CURLFORM_COPYNAME, "username",      //对应 input 标签中的 name 属性
                CURLFORM_COPYCONTENTS, "runoob",    //对应 input 标签中的 value 属性
                CURLFORM_END);
    curl_formadd(&formpost, &lastptr,
                CURLFORM_COPYNAME, "userage",
                CURLFORM_COPYCONTENTS, "3",
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
        curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.10.137/demo_formadd_1.php");
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
