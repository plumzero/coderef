#include <stdio.h>

/**
    回调函数 注册方法一
    纯粹地使用函数指针
 */
typedef int (*cysec_certverify_callback)(int crt, void* userdata);

static int tls_vcm_cb(int crt, void *userdata)
{
    printf("%s line=%d %d %s\n", __FUNCTION__, __LINE__, crt, (char*)userdata);
    return 0;
}

int cysec_tls_client_set_verify_callback(int* ctx, cysec_certverify_callback cb, void* userdata)
{
    printf("%s line=%d %d %s\n", __FUNCTION__, __LINE__, *ctx, (char*)userdata);
    cb(*ctx, userdata);
    printf("%s line=%d %p %s\n", __FUNCTION__, __LINE__, ctx, (char*)userdata);
    
    return 0;
}
/**
    回调函数 注册方法二
    函数指针+函数管理器(定义一个结构体)方式
 */

/**
    无论方法一还是方法二，本质是相同的
 */
int main()
{
    int i = 2;
    int* pi = &i;
    char str[] = { "testcb" };
    cysec_tls_client_set_verify_callback(pi, tls_vcm_cb, str);
    
    return 0;
}