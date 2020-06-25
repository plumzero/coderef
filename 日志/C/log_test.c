

#include <stdio.h>
#include "log.h"
         
#ifndef RTE_TEST_TRACE_FAILURE
#define RTE_TEST_TRACE_FAILURE(_file, _line, _func)
#endif


#define RTE_TEST_ASSERT(cond, msg, ...) do {                         \
    if (!(cond)) {                                                   \
        RTE_LOG(DEBUG, EAL, "Test assert %s line %d failed: "        \
                msg "\n", __func__, __LINE__, ##__VA_ARGS__);        \
        RTE_TEST_TRACE_FAILURE(__FILE__, __LINE__, __func__);        \
        return -1;                                                   \
    }                                                                \
} while (0)

#define RTE_TEST_ASSERT_EQUAL(a, b, msg, ...)        \
    RTE_TEST_ASSERT(a == b, msg, ##__VA_ARGS__)

#define RTE_TEST_ASSERT_NOT_EQUAL(a, b, msg, ...)    \
    RTE_TEST_ASSERT(a != b, msg, ##__VA_ARGS__)

#define RTE_TEST_ASSERT_SUCCESS(val, msg, ...)       \
    RTE_TEST_ASSERT(val == 0, msg, ##__VA_ARGS__)

#define RTE_TEST_ASSERT_FAIL(val, msg, ...)          \
    RTE_TEST_ASSERT(val != 0, msg, ##__VA_ARGS__)

#define RTE_TEST_ASSERT_NULL(val, msg, ...)           \
    RTE_TEST_ASSERT(val == NULL, msg, ##__VA_ARGS__)

#define RTE_TEST_ASSERT_NOT_NULL(val, msg, ...)       \
    RTE_TEST_ASSERT(val != NULL, msg, ##__VA_ARGS__)
         
/**
 * 对日志的简单测试。
 * 编译：
 *  gcc -g -O0 test.c log.c -o test
 * 内存泄漏测试（无内存泄漏）:
 *  valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./test
 */
int main()
{
    rte_log_init();
    
    RTE_LOG_EX(DEBUG, EAL, "sysfs is not mounted! error %i (%s)\n", -1, "debug output test");
    
    RTE_LOG_EX(INFO, EAL, "sysfs is not mounted! error %i (%s)\n", -1, "info output test");
    
    rte_log_set_level(RTE_LOGTYPE_EAL, RTE_LOG_DEBUG);
    RTE_LOG_EX(DEBUG, EAL, "sysfs is not mounted! error %i (%s)\n", -1, "debug output test");
    
    rte_log_free();
    
    return 0;
}
