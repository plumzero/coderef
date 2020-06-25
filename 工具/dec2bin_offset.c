#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 采取链式栈实现方法
typedef struct _node
{
    char data;
    struct _node * next;
}Node;

typedef struct stack_
{
    int size;
    Node * top;
}Stack;

Stack * stack_init()
{
    Stack * st = calloc(1, sizeof(Stack));
    if (st == NULL)
        return NULL;
    st->size = 0;
    st->top = NULL;
    
    return st;
}

int stack_push(Stack * st, char ch)
{
    Node * node = calloc(1, sizeof(Node));
    if (node == NULL)
        return -1;

    node->data = ch;
    node->next = st->top;
    st->top = node;
    st->size++;
    
    return 0;
}

int stack_pop(Stack * st, char * ch)
{
    Node * node = st->top;
    st->top = node->next;
    
    *ch = node->data;
    free(node);

    st->size--;
    
    return 0;
}

void stack_free(Stack * st)
{
    while (st->top)
    {
        Node * node = st->top;
        st->top = node->next;
        free(node);
    }
    
    if (st){
        free(st);
        st = NULL;
    }
}

void stack_traverse(Stack * st)
{
    Node * node = st->top;
    // printf("size: %d\n", st->size);
    while (node)
    {
        printf("%c", node->data);
        node = node->next;
    }
}
int dec2bin(int dec, unsigned char * bin, unsigned int binsize)
{
    (void)bin;
    (void)binsize;
    
    Stack * st = stack_init();
    int bit;
    while (dec){
        bit = dec % 2;
        dec = dec / 2;
        if (bit == 1)
            stack_push(st, '1');
        else
            stack_push(st, '0');
    }
    stack_traverse(st);
    return 0;
}

/**
 *  输入一个十进制数，将其转为二进制串，并指定一个偏移位数，输出偏移后的二进制串及十进制数。
 */

int main()
{
    int num, orient, offset, err;
    unsigned char bin[512];
    
    while (1)
    {
        printf("输入十进制数：");
        err = scanf("%d", &num);
        if (err == 0)
        {
            printf("输入不匹配");
            while(getchar()!='\n'){};
            continue;
        }
        // 十进制转二进制
        printf("%d -> ", num);
        memset(bin, 0, sizeof(bin));
        dec2bin(num, bin, sizeof(bin));
        printf("\n");
        // 进行偏移
        printf("指定偏移模式：偏移方向(左0右1) 偏移位数\n");
        err = scanf("%d %d", &orient, &offset);
        if (err == 0)
        {
            printf("输入不匹配");
            while(getchar()!='\n'){};
            continue;
        }
        printf("偏移方向: %s\t偏移位数: %d\n", orient ? "右" : "左", offset);
        num = orient ? num >> offset : num << offset;
        printf("%d -> ", num);
        memset(bin, 0, sizeof(bin));
        dec2bin(num, bin, sizeof(bin));
        printf("\n\n");
    }
    
    return 0;
}