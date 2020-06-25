
#include <stdio.h>
#include "cxxlog.h"

#define V2X_LOG(logger, l, t, ...)                           \
    do {                                                     \
        logger->log_out(LOG_LEVEL_ ## l, LOG_TYPE_ ## t,     \
                        "[" # l "] " __VA_ARGS__);           \
    } while (0)

#define V2X_LOG_EX(logger, l, t, msg, ...)                                             \
    do {                                                                               \
        V2X_LOG(logger, l, t, "%03d " msg, __LINE__, ##__VA_ARGS__);                   \
    } while(0)

#define V2X_ERR_LOG(assertion, ret, retval, label, logger, type, format, ...)                      \
    do {                                                                                           \
        if (!(assertion)) {                                                                       \
            V2X_LOG(logger, DBUG, type, "%03d pass\n", __LINE__);        \
        } else {                                                                       \
            V2X_LOG(logger, ERRO, type, LOG_COLOR_RED "%03d " format "\n" LOG_COLOR_NONE,  \
                                                    __LINE__, ##__VA_ARGS__);          \
            ret = retval;                                                              \
            goto label;                                                                \
        }                                                                              \
    } while (0)
    
#define V2X_WARN_LOG(logger, format, ...)                                              \
    do {                                                                               \
        V2X_LOG(logger, WARN, GLOBAL, LOG_COLOR_PINK "%4d " format "\n" LOG_COLOR_NONE,\
                                                          __LINE__, ##__VA_ARGS__);    \
    } while (0)

#define ERR_BASE                -5000
#define ERR_MALLOC              ERR_BASE + 1
#define ERR_POSIX               ERR_BASE + 2
#define ERR_NOT_EQUAL           ERR_BASE + 5

/**
 * 对日志的简单测试。
 * 日志格式：
 *  [时间] [等级] [定位] [详细信息]
 * 编译：
 *  g++ -g -O0 cxxtest.c cxxlog.c -o test
 * 内存泄漏测试（无明显内存泄漏）:
 *  valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./test
 */
int main()
{
    Logger *_logger = new Logger("./", "v2x.log");
    
    /// ****************************** 等级测试 ****************************** ///
    // printf("注册的日志表内容:\n");
    // logger->log_print_regtab();
    V2X_LOG_EX(_logger, DBUG, MEMORY, "%s\n", "debug > MEMORY type level(info), do not output");
    V2X_LOG_EX(_logger, INFO, MEMORY, "%s\n", "info = MEMORY type level(info), output");
    V2X_LOG_EX(_logger, NOTI, MEMORY, "%s\n", "notice < MEMORY type level(info), output");
    _logger->log_set_level(LOG_TYPE_MEMORY, LOG_LEVEL_ERRO);
    _logger->log_set_level(LOG_TYPE_ASN1, LOG_LEVEL_NOTI);
    _logger->log_set_level(LOG_TYPE_PARAM, LOG_LEVEL_WARN);
    _logger->log_set_level(LOG_TYPE_POSIX, LOG_LEVEL_ERRO);
    // printf("注册的日志表内容:\n");
    // _logger->log_print_regtab();
    V2X_LOG_EX(_logger, DBUG, MEMORY, "%s\n", "debug = MEMORY type level(debug), output");
    V2X_LOG_EX(_logger, NOTI, MEMORY, "%s\n", "notice < MEMORY type level(debug), output");
    V2X_LOG_EX(_logger, ERRO, ASN1, "%s\n", "err < ASN1 type level(notice), output");
    V2X_LOG_EX(_logger, INFO, PARAM, "%s\n", "info > PARAM type level(warning), do not output");
    V2X_LOG_EX(_logger, CRIT, POSIX, "%s\n", "critical < POSIX type level(err), output");

    V2X_WARN_LOG(_logger, "ec couldn't be verified by ec signer, issue ec, , but please replace ec signer soon");

    /// ****************************** 切换日志测试 ****************************** ///
    /// 通过

    /// ****************************** 移植测试 ****************************** ///
    int i = 10, j = 20, ret;
    char *p = NULL;
    FILE* pf = NULL;
    p = (char*)calloc(1024, sizeof(char));
    V2X_ERR_LOG(p == NULL, ret, ERR_MALLOC, cleanup, _logger, MEMORY, "%s", "calloc failed");
    
    i = j = 100;
    V2X_ERR_LOG(i != j, ret, ERR_NOT_EQUAL, cleanup, _logger, PARAM, "%d is not equal to %d", i, j);
    // 开启调试，显示输出
    _logger->log_set_level(LOG_TYPE_PARAM, LOG_LEVEL_DBUG);
    V2X_ERR_LOG(i != j, ret, ERR_NOT_EQUAL, cleanup, _logger, PARAM, "%d is not equal to %d", i, j);
    // 仅错误时显示
    _logger->log_set_level(LOG_TYPE_PARAM, LOG_LEVEL_ERRO);
    V2X_ERR_LOG(i != j, ret, ERR_NOT_EQUAL, cleanup, _logger, PARAM, "%d is not equal to %d", i, j);
    
    pf = fopen("not_exist.txt", "rb");
    V2X_ERR_LOG(pf == NULL, ret, ERR_POSIX, cleanup, _logger, POSIX, "%s", "fopen(rb) failed");
    
    ret = 0;
cleanup:

    printf("========== test ok ==========\n");
    if (p) free(p);
    if (pf) fclose(pf);
    delete _logger;
    
    return ret;
}
