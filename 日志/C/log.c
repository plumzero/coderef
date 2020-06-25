
#include "log.h"

static FILE *default_log_stream;
 
// 全局结构体
struct rte_logs rte_logs = {
    .type = ~0,
    .level = RTE_LOG_DEBUG,
    .file = NULL,
};

int rte_openlog_stream(FILE *f)
{
    rte_logs.file = f;
    return 0;
}

void rte_log_set_global_level(uint32_t level)
{
    rte_logs.level = (uint32_t)level;
}

uint32_t rte_log_get_global_level(void)
{
    return rte_logs.level;
}

int rte_log_get_level(uint32_t type)
{
    if (type >= rte_logs.dynamic_types_len)
        return -1;

    return rte_logs.dynamic_types[type].loglevel;
}
 
int rte_log_set_level(uint32_t type, uint32_t level)
{
    if (type >= rte_logs.dynamic_types_len)
        return -1;
    if (level > RTE_LOG_DEBUG)
        return -1;

    rte_logs.dynamic_types[type].loglevel = level;

    return 0;
}

static int rte_log_lookup(const char *name)
{
    size_t i;

    for (i = 0; i < rte_logs.dynamic_types_len; i++) {
        if (rte_logs.dynamic_types[i].name == NULL)
            continue;
        if (strcmp(name, rte_logs.dynamic_types[i].name) == 0)
            return i;
    }

    return -1;
}

// 注册一个扩展日志类型，假定表足够大，id 未被注册
static int __rte_log_register(const char *name, int id)
{
    char *dup_name = strdup(name);

    if (dup_name == NULL)
        return -ENOMEM;

    rte_logs.dynamic_types[id].name = dup_name;
    rte_logs.dynamic_types[id].loglevel = RTE_LOG_INFO;

    return id;
}

// 注册一个扩展日志类型
int rte_log_register(const char *name)
{
    struct rte_log_dynamic_type *new_dynamic_types;
    int id, ret;

    id = rte_log_lookup(name);
    if (id >= 0)
        return id;

    new_dynamic_types = realloc(rte_logs.dynamic_types,
        sizeof(struct rte_log_dynamic_type) *
        (rte_logs.dynamic_types_len + 1));
    if (new_dynamic_types == NULL)
        return -ENOMEM;
    rte_logs.dynamic_types = new_dynamic_types;

    ret = __rte_log_register(name, rte_logs.dynamic_types_len);
    if (ret < 0)
        return ret;

    rte_logs.dynamic_types_len++;

    return ret;
}

// 日志初始化
void rte_log_init(void)
{
    uint32_t i;

    rte_log_set_global_level(RTE_LOG_DEBUG);

    rte_logs.dynamic_types = calloc(RTE_LOGTYPE_FIRST_EXT_ID,
        sizeof(struct rte_log_dynamic_type));
    if (rte_logs.dynamic_types == NULL)
        return;

    // 注册日志类型
    for (i = 0; i < RTE_DIM(logtype_strings); i++)
        __rte_log_register(logtype_strings[i].logtype,
                logtype_strings[i].log_id);

    rte_logs.dynamic_types_len = RTE_LOGTYPE_FIRST_EXT_ID;
}

// 日志释放
void rte_log_free(void)
{
    uint32_t i;
    
    for (i = 0; i < RTE_DIM(logtype_strings); i++) {
        uint32_t id = logtype_strings[i].log_id;
        if (rte_logs.dynamic_types[id].name)
            free((void*)(rte_logs.dynamic_types[id].name));
        rte_logs.dynamic_types[id].name = NULL;
    }

    if (rte_logs.dynamic_types) 
        free(rte_logs.dynamic_types);
    rte_logs.dynamic_types = NULL;
    
    memset(&rte_logs, 0, sizeof(struct rte_logs));
}

int rte_vlog(uint32_t level, uint32_t logtype, const char *format, va_list ap)
{
    int ret;
    FILE *f = rte_logs.file;
    if (f == NULL) {
        f = default_log_stream;
        if (f == NULL) {
            f = stderr;
        }
    }

    if (level > rte_logs.level)
        return 0;
    if (logtype >= rte_logs.dynamic_types_len)
        return -1;
    if (level > rte_logs.dynamic_types[logtype].loglevel)
        return 0;

    ret = vfprintf(f, format, ap);
    fflush(f);
    return ret;
}

int rte_log(uint32_t level, uint32_t logtype, const char *format, ...)
{
    va_list ap;
    int ret;

    va_start(ap, format);
    ret = rte_vlog(level, logtype, format, ap);
    va_end(ap);
    return ret;
}
