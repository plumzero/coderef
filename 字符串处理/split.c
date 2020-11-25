
#include <string.h>
#include <stdio.h>
#include <errno.h>

/**
 * 字符串缓冲中裂开
 */
int split(char *string, int stringlen, char **tokens, int maxtokens, char delim)
{
    int i, tok = 0;
    int tokstart = 1; /* first token is right at start of string */

    if (string == NULL || tokens == NULL)
        goto einval_error;

    for (i = 0; i < stringlen; i++) {
        if (string[i] == '\0' || tok >= maxtokens)
            break;
        if (tokstart) {
            tokstart = 0;
            tokens[tok++] = &string[i];
        }
        if (string[i] == delim) {
            string[i] = '\0';
            tokstart = 1;
        }
    }
    return (int)tok;

einval_error:
    errno = EINVAL;
    return -1;
}

int main()
{
    char *tokens[100] = { 0 };
    
    char buf[] = { " $(CONSOLE_SRC_PATH)/../ oss/dynamic_bitset; / " };
    
    int toks = split(buf, strlen(buf), tokens, sizeof(tokens) / sizeof(void*), '/');
    
    printf("toks=%d\n", toks);
    
    int i;
    for (i = 0; i < toks; i++) {
        printf("tokens[%d]={%s}\n", i, tokens[i]);
    }
    
    return 0;
}