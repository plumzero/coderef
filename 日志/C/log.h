

/**
 * 名词解释：
 * log type                    日志类型
 * log level                   日志等级
 */

#ifndef __LOG_H__
#define __LOG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

struct rte_log_dynamic_type {
    const char *name;
    uint32_t loglevel;
};

struct rte_logs {
    uint32_t type;      // 日志类型比特
    uint32_t level;     // 日志等级
    FILE *file;         // 流环境或空
    size_t dynamic_types_len;
    struct rte_log_dynamic_type *dynamic_types;
};

struct logtype {
    uint32_t log_id;
    const char *logtype;
};

// 设置日志为全局
extern struct rte_logs rte_logs;

#define RTE_DIM(a)  (sizeof (a) / sizeof ((a)[0]))

/**
 * 针对每种日志类型指定不同日志等级。
 * 1. 规定：任何日志类型所对应的日志等级不得大于全局日志等级；
 * 2. 在实际生产中，若所要求的：
 *    输出日志等级 > 全局日志等级， 不输出
 *    输出日志等级 > 该日志类型等级，不输出
 *    即只输出不大于该日志类型所规定日志等级的日志。
 * 3. 通常，将全局日志等级设置为最高等级，这里是 RTE_LOG_DEBUG ，且一般中间不作任何改变。
 *    可通过改变每种日志类型所对应的日志等级来区别开发或生产的输出。
 * 
 */
// SDK 日志类型标识符
#define RTE_LOGTYPE_EAL        0 /**< Log related to eal. */
#define RTE_LOGTYPE_MALLOC     1 /**< Log related to malloc. */
#define RTE_LOGTYPE_RING       2 /**< Log related to ring. */
#define RTE_LOGTYPE_MEMPOOL    3 /**< Log related to mempool. */
#define RTE_LOGTYPE_TIMER      4 /**< Log related to timers. */
#define RTE_LOGTYPE_PMD        5 /**< Log related to poll mode driver. */
#define RTE_LOGTYPE_HASH       6 /**< Log related to hash table. */
#define RTE_LOGTYPE_LPM        7 /**< Log related to LPM. */
#define RTE_LOGTYPE_KNI        8 /**< Log related to KNI. */
#define RTE_LOGTYPE_ACL        9 /**< Log related to ACL. */
#define RTE_LOGTYPE_POWER     10 /**< Log related to power. */
#define RTE_LOGTYPE_METER     11 /**< Log related to QoS meter. */
#define RTE_LOGTYPE_SCHED     12 /**< Log related to QoS port scheduler. */
#define RTE_LOGTYPE_PORT      13 /**< Log related to port. */
#define RTE_LOGTYPE_TABLE     14 /**< Log related to table. */
#define RTE_LOGTYPE_PIPELINE  15 /**< Log related to pipeline. */
#define RTE_LOGTYPE_MBUF      16 /**< Log related to mbuf. */
#define RTE_LOGTYPE_CRYPTODEV 17 /**< Log related to cryptodev. */
#define RTE_LOGTYPE_EFD       18 /**< Log related to EFD. */
#define RTE_LOGTYPE_EVENTDEV  19 /**< Log related to eventdev. */
#define RTE_LOGTYPE_GSO       20 /**< Log related to GSO. */
// 应用日志类型标识符
#define RTE_LOGTYPE_USER1     24 /**< User-defined log type 1. */
#define RTE_LOGTYPE_USER2     25 /**< User-defined log type 2. */
#define RTE_LOGTYPE_USER3     26 /**< User-defined log type 3. */
#define RTE_LOGTYPE_USER4     27 /**< User-defined log type 4. */
#define RTE_LOGTYPE_USER5     28 /**< User-defined log type 5. */
#define RTE_LOGTYPE_USER6     29 /**< User-defined log type 6. */
#define RTE_LOGTYPE_USER7     30 /**< User-defined log type 7. */
#define RTE_LOGTYPE_USER8     31 /**< User-defined log type 8. */
// 扩展日志类型首标识符
#define RTE_LOGTYPE_FIRST_EXT_ID 32

// 日志等级
#define RTE_LOG_EMERG    1U  /**< System is unusable.               */
#define RTE_LOG_ALERT    2U  /**< Action must be taken immediately. */
#define RTE_LOG_CRIT     3U  /**< Critical conditions.              */
#define RTE_LOG_ERR      4U  /**< Error conditions.                 */
#define RTE_LOG_WARNING  5U  /**< Warning conditions.               */
#define RTE_LOG_NOTICE   6U  /**< Normal but significant condition. */
#define RTE_LOG_INFO     7U  /**< Informational.                    */
#define RTE_LOG_DEBUG    8U  /**< Debug-level messages.             */

static const struct logtype logtype_strings[] = {
    {RTE_LOGTYPE_EAL,        "lib.eal"},
    {RTE_LOGTYPE_MALLOC,     "lib.malloc"},
    {RTE_LOGTYPE_RING,       "lib.ring"},
    {RTE_LOGTYPE_MEMPOOL,    "lib.mempool"},
    {RTE_LOGTYPE_TIMER,      "lib.timer"},
    {RTE_LOGTYPE_PMD,        "pmd"},
    {RTE_LOGTYPE_HASH,       "lib.hash"},
    {RTE_LOGTYPE_LPM,        "lib.lpm"},
    {RTE_LOGTYPE_KNI,        "lib.kni"},
    {RTE_LOGTYPE_ACL,        "lib.acl"},
    {RTE_LOGTYPE_POWER,      "lib.power"},
    {RTE_LOGTYPE_METER,      "lib.meter"},
    {RTE_LOGTYPE_SCHED,      "lib.sched"},
    {RTE_LOGTYPE_PORT,       "lib.port"},
    {RTE_LOGTYPE_TABLE,      "lib.table"},
    {RTE_LOGTYPE_PIPELINE,   "lib.pipeline"},
    {RTE_LOGTYPE_MBUF,       "lib.mbuf"},
    {RTE_LOGTYPE_CRYPTODEV,  "lib.cryptodev"},
    {RTE_LOGTYPE_EFD,        "lib.efd"},
    {RTE_LOGTYPE_EVENTDEV,   "lib.eventdev"},
    {RTE_LOGTYPE_GSO,        "lib.gso"},
    {RTE_LOGTYPE_USER1,      "user1"},
    {RTE_LOGTYPE_USER2,      "user2"},
    {RTE_LOGTYPE_USER3,      "user3"},
    {RTE_LOGTYPE_USER4,      "user4"},
    {RTE_LOGTYPE_USER5,      "user5"},
    {RTE_LOGTYPE_USER6,      "user6"},
    {RTE_LOGTYPE_USER7,      "user7"},
    {RTE_LOGTYPE_USER8,      "user8"}
};

/**
 * @desc
 *  日志全局初始化。
 */
void rte_log_init(void);

/**
 * @desc
 *  日志全局释放。
 */
void rte_log_free(void);

/**
 * @desc
 *  改变日志系统流环境。
 *  此操作可以在任何时段进行，如果 f 为空，则为标准错误输出。
 * @param f
 *  流指针
 * @return
 *  成功 0
 */
int rte_openlog_stream(FILE* f);

/**
 * @desc
 *  设置全局日志等级。
 *  此操作后，低于等于设置等级的信息将会被打印。
 * @param level
 *  日志等级，介于 RTE_LOG_EMERG(1) 和 RTE_LOG_DEBUG(8) 之间
 */
void rte_log_set_global_level(uint32_t level);

/**
 * @desc
 *  获取全局日志等级。
 * @return
 *  返回当前日志等级
 */
uint32_t rte_log_get_global_level(void);

/**
 * @desc
 *  获取指定日志类型的日志等级
 * @param logtype
 *  日志类型标识符
 * @return
 *  成功 0
 */
int rte_log_get_level(uint32_t logtype);

/**
 * @desc
 *  为指定日志类型设置日志等级
 * @param logtype
 *  日志类型标识符
 * @param level
 *  日志等级
 * @return
 *  成功 0
 */
int rte_log_set_level(uint32_t logtype, uint32_t level);

/**
 * @desc
 *  创建日志输出。
 * @param level
 *  日志等级
 * @param format
 *  格式化字符串，正如 printf(3)，其后紧跟可变参数
 * @return
 *  成功 0
 */
int rte_log(uint32_t level, uint32_t logtype, const char *format, ...)
#ifdef __GNUC__
#if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 2))
    __attribute__((cold))
#endif
#endif
    __attribute__((format(printf, 3, 4)));
    
/**
 * @desc
 *  日志输出帮助宏。
 * @param l
 *  日志等级
 * @param t
 *  日志类型
 * @param ...
 *  格式化字符串
 * @return
 *  成功 0
 */
#define RTE_LOG(l, t, ...)                  \
     rte_log(RTE_LOG_ ## l,                 \
         RTE_LOGTYPE_ ## t, # t ": " __VA_ARGS__)

/**
 * @desc
 *  增强宏。
 */
#define RTE_LOG_EX(l, t, ...)                                              \
     do {                                                                  \
     rte_log(RTE_LOG_ ## l,                                                \
             RTE_LOGTYPE_ ## t, "%s:%-4d ", __FUNCTION__, __LINE__);       \
     rte_log(RTE_LOG_ ## l,                                                \
             RTE_LOGTYPE_ ## t, "[" # l "] [" # t "] " __VA_ARGS__);       \
     } while (0)
 
#ifdef __cplusplus
}
#endif

#endif // !__LOG_H__