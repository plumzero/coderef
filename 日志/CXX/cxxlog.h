
#ifndef __V2X_LOG_H__
#define __V2X_LOG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <errno.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

/**
 * 针对每种日志类型指定不同日志等级。
 * 1. 规定：任何日志类型所对应的日志等级不得大于全局日志等级；
 * 2. 在实际生产中，若所要求的：
 *    输出日志等级 > 全局日志等级， 不输出
 *    输出日志等级 > 该日志类型等级，不输出
 *    即只输出不大于该日志类型所规定日志等级的日志。
 * 3. 通常，将全局日志等级设置为最高等级，这里是 LOG_LEVEL_DEBUG ，且一般中间不作任何改变。
 *    通常，不会将具体日志等级设置为最高等级
 *    可通过改变每种日志类型所对应的日志等级来区别调试或生产的输出。
 * 
 */
// 基础日志类型标识符索引
#define LOG_TYPE_GLOBAL     0      /** 全局信息相关        */
#define LOG_TYPE_MEMORY     1      /** 内存相关            */
#define LOG_TYPE_PIPELINE   2      /** 管道相关            */
#define LOG_TYPE_ASN1       3      /** asn1编解码相关      */
#define LOG_TYPE_IO         4      /** i/o 相关            */
#define LOG_TYPE_PARAM      5      /** 传入参数相关        */
#define LOG_TYPE_POSIX      6      /** posix 函数相关      */
#define LOG_TYPE_SDF        7      /** sdf 函数相关        */
#define LOG_TYPE_HTTP       8      /** http 相关           */
#define LOG_TYPE_BUFFER     9      /** 缓存相关            */
#define LOG_TYPE_CHECK     10      /** 校验相关            */
// 应用日志类型标识符索引
#define LOG_TYPE_ENCRYPT    21     /** 加密相关            */
#define LOG_TYPE_DECRYPT    22     /** 解密相关            */
#define LOG_TYPE_SIGN       23     /** 签名相关            */
#define LOG_TYPE_VERIFY     24     /** 验签相关            */
#define LOG_TYPE_ENVELOPE   25     /** 信封相关            */
#define LOG_TYPE_CRL        26     /** crl 相关            */
// 扩展日志类型首标识符索引
#define LOG_TYPE_FIRST_EXT_ID 32
// 着色
#define LOG_COLOR_NONE              "\033[0;m"          /** 重置   */
#define LOG_COLOR_RED               "\033[1;31m"        /** 红色   */
#define LOG_COLOR_GREEN             "\033[1;32m"        /** 绿色   */
#define LOG_COLOR_YELLOW            "\033[1;33m"        /** 黄色   */
#define LOG_COLOR_BLUE              "\033[1;34m"        /** 蓝色   */
#define LOG_COLOR_PINK              "\033[1;35m"        /** 洋红色 */
#define LOG_COLOR_INDIGO            "\033[1;36m"        /** 青色   */

// 日志等级
#define LOG_LEVEL_EMER     1U  /** 不打印任何输出          */
#define LOG_LEVEL_ALER     2U  /** 需要立即做出相应动作    */
#define LOG_LEVEL_CRIT     3U  /** 严格条件触发            */
#define LOG_LEVEL_ERRO     4U  /** 错误条件触发            */
#define LOG_LEVEL_WARN     5U  /** 警告条件触发            */
#define LOG_LEVEL_NOTI     6U  /** 运行正常，但建议...     */
#define LOG_LEVEL_INFO     7U  /** 正常打印必要信息        */
#define LOG_LEVEL_DBUG     8U  /** 调试打印冗余信息        */

struct log_type {
    const char *log_type_name;    /** log 类型对应的字符串名称，与 log_reg_table 一致，用于查找 */
    uint32_t    log_type_level;   /** log 类型对应的日志等级 */
};

struct log_type_reg_table {
    uint32_t    log_type_id;          /** log 类型的注册的 id 标识，最大不得超过 LOG_TYPE_FIRST_EXT_ID */
    const char *log_type_name;        /** log 类型的注册的字符串名称 */
};

class Logger
{
public:
    Logger(const char* dir, 
           const char* name, 
           uint32_t limit_size = 1, 
           uint32_t level = LOG_LEVEL_DBUG);
    ~Logger();
public:
    void      log_init(void);
    void      log_free(void);
    int       log_open_stream(FILE* f);
    void      log_set_global_level(uint32_t level);
    uint32_t  log_get_global_level(void);
    int       log_get_level(uint32_t logtype);
    int       log_set_level(uint32_t logtype, uint32_t level);
    int       log_register(const char *name);
    void      log_print_regtab();
    int       log_out(uint32_t level, uint32_t logtype, const char *format, ...);
private:
    int       log_lookup(const char *name);
    int       log_vlog(uint32_t level, uint32_t logtype, const char *format, va_list ap);
    int       log_register_internal(const char *name, int id);

private:
    uint32_t         m_nTypeMask;        // 全局日志类型比特掩码（暂无用）
    uint32_t         m_nLevel;           // 全局日志等级，一般情况下将该值设置为最高等级 LOG_LEVEL_DEBUG
    const char*      m_szPath;           // 全局日志所在绝对路径
    uint32_t         m_nLimitSize;       // 全局日志尺寸限制（以 m 为单位）
    FILE*            m_pFile;            // 全局流环境或空
    struct log_type* m_pLogTypes;        // 具体日志类型存储数组，位置索引与 LOG_TYPE_* 一一对应
    size_t           m_nLogTypesSize;    // 具体日志类型存储数组大小
};

// 日志类型注册表
static const struct log_type_reg_table reg_table[] = {
    { LOG_TYPE_GLOBAL,       "v2x.global" },
    { LOG_TYPE_MEMORY,       "v2x.memory" },
    { LOG_TYPE_PIPELINE,     "v2x.pipeline" },
    { LOG_TYPE_ASN1,         "v2x.asn1" },
    { LOG_TYPE_IO,           "v2x.io" },
    { LOG_TYPE_PARAM,        "v2x.param" },
    { LOG_TYPE_POSIX,        "v2x.posix" },
    { LOG_TYPE_SDF,          "v2x.sdf" },
    { LOG_TYPE_HTTP,         "v2x.http" },
    { LOG_TYPE_BUFFER,       "v2x.buffer" },
    { LOG_TYPE_CHECK,        "v2x.check" },
    
    { LOG_TYPE_ENCRYPT,      "v2x.encrypt" },
    { LOG_TYPE_DECRYPT,      "v2x.decrypt" },
    { LOG_TYPE_SIGN,         "v2x.sign" },
    { LOG_TYPE_VERIFY,       "v2x.verify" },
    { LOG_TYPE_ENVELOPE,     "v2x.envelope" },
    { LOG_TYPE_CRL,          "v2x.crl" }
};


#ifdef __cplusplus
}
#endif

#endif // ! __V2X_LOG_H__