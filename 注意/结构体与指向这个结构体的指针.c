#include <stdio.h>
#include <stdlib.h>

typedef struct ClientInfo   ClientInfo;
typedef struct GlobalInfo   GlobalInfo;

struct ClientInfo{
    ClientInfo * next;
    int use;
};
struct GlobalInfo{
    ClientInfo * cli_info;
};
int main()
{   
    GlobalInfo *g = (GlobalInfo*)calloc(1, sizeof(GlobalInfo));
    ClientInfo *c1 = NULL, *c2 = NULL, *p = NULL;
    
    if (g == NULL)
    {
        printf("-----%d\n", __LINE__);
        goto err;
    }
    c1 = (ClientInfo*)calloc(1, sizeof(ClientInfo));
    if (g == NULL)
    {
        printf("-----%d\n", __LINE__);
        goto err;
    }
    else
    {
        c1->use = 1;
        printf("++++++%d\n", c1->use);
    }
    c2 = (ClientInfo*)calloc(1, sizeof(ClientInfo));
    if (g == NULL)
    {
        printf("-----%d\n", __LINE__);
        goto err;
    }
    else
    {
        c2->use = 2;
        printf("++++++%d\n", c2->use);
    }
    
    //下面一句不能写成 g->cli_info->next = c1; 不能把ClientInfo*指针类型的空间当作ClientInfo结构体空间来使用
    g->cli_info = c1;
    c1->next = c2;
    c2->next = NULL;
    
    p = g->cli_info;
    while (p)
    {
        printf("---%d---\n", p->use);
        p = p->next;
    }
err:
    if (g) free(g);
    if (c1) free(c1);
    if (c2) free(c2);
    return 0;
}