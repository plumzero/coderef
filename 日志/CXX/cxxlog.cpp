
#include "cxxlog.h"

Logger::Logger(const char* dir, 
               const char* name,
               uint32_t limit_size,
               uint32_t level) { 
    
    char path[PATH_MAX] = { 0 };
    char bakpath[PATH_MAX] = { 0 };
    FILE* pf;
    int ret;
    uint32_t fsize;
    strcpy(path, dir);
    if (access(path, W_OK) != 0) {
        ret = mkdir(path, 0666);
        if (ret < 0 && errno != EEXIST) {
            fprintf(stderr, "could not create a folder for log.\n");
            throw;
        }
    }
    if (path[strlen(path) - 1] != '/')
        strcat(path, "/");
    strcpy(bakpath, path);
    strcat(path, name);
    if (access(path, F_OK) == 0) {
        pf = fopen(path, "rb");
        fseek(pf, 0, SEEK_END);
        fsize = ftell(pf);
        fseek(pf, 0, SEEK_SET);
        fclose(pf);
        if (fsize > limit_size * 1024 * 1024) {
            strcat(bakpath, "old.log");
            if (access(bakpath, F_OK) == 0) {
                if (unlink(bakpath) != 0) {
                    fprintf(stderr, "could not delete the old log.\n");
                    throw;
                }
            }
            if (rename(path, bakpath) != 0) {
                fprintf(stderr, "could not back-up log.\n");
                throw;
            }
        }
    }
    pf = fopen(path, "a+");
    if (pf == NULL) {
        fprintf(stderr, "could not open the log.\n");
        throw;
    }
    if (level > LOG_LEVEL_DBUG || level < LOG_LEVEL_EMER) {
        fprintf(stderr, "log level is invalid.\n");
        throw;
    }
    log_set_global_level(level);
    m_szPath = strdup(path);
    m_nLimitSize = limit_size;
    m_pFile = pf;
    m_nTypeMask = (uint32_t)~0;
    log_init();
}

Logger::~Logger() { 
    log_free();
    free((void*)m_szPath);
    fclose(m_pFile);
}

/**
 * @desc
 *  改变日志系统流环境。
 *  此操作可以在任何时段进行，如果 f 为空，则为标准错误输出。
 * @param f
 *  流指针
 * @return
 *  成功 0
 */
int Logger::log_open_stream(FILE *f)
{
    m_pFile = f;
    return 0;
}
/**
 * @desc
 *  设置全局日志等级。
 *  此操作后，低于等于设置等级的信息将会被打印。
 * @param level
 *  日志等级，介于 LOG_LEVEL_EMERG(1) 和 LOG_LEVEL_DEBUG(8) 之间
 */
void Logger::log_set_global_level(uint32_t level)
{
    m_nLevel = (uint32_t)level;
}
/**
 * @desc
 *  获取全局日志等级。
 * @return
 *  返回当前日志等级
 */
uint32_t Logger::log_get_global_level(void)
{
    return m_nLevel;
}
/**
 * @desc
 *  获取指定日志类型的日志等级
 * @param logtype
 *  日志类型标识符索引
 * @return
 *  成功 0
 */
int Logger::log_get_level(uint32_t logtype)
{
    if (logtype >= m_nLogTypesSize)
        return -1;

    return m_pLogTypes[logtype].log_type_level;
}
/**
 * @desc
 *  为指定日志类型设置日志等级
 * @param logtype
 *  日志类型标识符索引
 * @param loglevel
 *  日志类型的等级
 * @return
 *  成功 0
 */
int Logger::log_set_level(uint32_t logtype, uint32_t loglevel)
{
    if (logtype >= m_nLogTypesSize)
        return -1;
    if (loglevel > LOG_LEVEL_DBUG)
        return -1;

    m_pLogTypes[logtype].log_type_level = loglevel;

    return 0;
}
/**
 * @desc
 *  根据字符串查找对应的日志类型
 * @param name
 *  字符串名称
 * @return
 *  成功 返回对应的日志类型的标识符索引
 *  失败 -1
 */
int Logger::log_lookup(const char *name)
{
    size_t i;

    for (i = 0; i < m_nLogTypesSize; i++) {
        if (m_pLogTypes[i].log_type_name == NULL)
            continue;
        if (strcmp(name, m_pLogTypes[i].log_type_name) == 0)
            return i;
    }

    return -1;
}

/**
 * @desc 
 *  注册一个扩展日志类型，假定表足够大，id 未被注册。缺省时注册的日志类型的等级为 INFO
 * @param name
 *  日志类型对应的字符串名称，如 "v2x.???"
 * @param id
 *  日志类型对应的标识符索引
 * @return
 *  成功 返回对应的日志类型的标识符索引
 *  失败
 */
int Logger::log_register_internal(const char *name, int id)
{
    char *dup_name = strdup(name);

    if (dup_name == NULL)
        return -ENOMEM;

    m_pLogTypes[id].log_type_name = dup_name;
    m_pLogTypes[id].log_type_level = LOG_LEVEL_INFO;

    return id;
}

/**
 * @desc 
 *  注册一个扩展日志类型
 * @param name
 *  日志类型对应的字符串名称，如 "v2x.???"
 * @return
 *  成功 返回对应的日志类型的标识符索引
 *  失败
 */
int Logger::log_register(const char *name)
{
    struct log_type *new_log_types;
    int id, ret;

    id = log_lookup(name);
    if (id >= 0)
        return id;

    new_log_types = (struct log_type *)realloc(m_pLogTypes, sizeof(struct log_type) * (m_nLogTypesSize + 1));
    if (new_log_types == NULL)
        return -ENOMEM;
    m_pLogTypes = new_log_types;

    ret = log_register_internal(name, m_nLogTypesSize);
    if (ret < 0)
        return ret;

    m_nLogTypesSize++;

    return ret;
}

void Logger::log_print_regtab()
{
    uint32_t i;
    const char* level;
    
    for (i = 0; i < m_nLogTypesSize; i++) {
        if (m_pLogTypes[i].log_type_name) {
            printf("%02d: %-20s ", i, m_pLogTypes[i].log_type_name);
            switch (m_pLogTypes[i].log_type_level) {
                case LOG_LEVEL_EMER:
                    level = "EMERG";
                    break;
                case LOG_LEVEL_ALER:
                    level = "ALERT";
                    break;
                case LOG_LEVEL_CRIT:
                    level = "CRITICAL";
                    break;
                case LOG_LEVEL_ERRO:
                    level = "ERROR";
                    break;
                case LOG_LEVEL_WARN:
                    level = "WARNING";
                    break;
                case LOG_LEVEL_NOTI:
                    level = "NOTICE";
                    break;
                case LOG_LEVEL_INFO:
                    level = "INFO";
                    break;
                case LOG_LEVEL_DBUG:
                    level = "DEBUG";
                    break;
                default:
                    level = "";
                    break;
            }
            if (strcmp(level, "")) printf("%-10s\n", level);
        } else {
            printf("%02d: \n", i);
        }
    }
}

// 日志初始化
void Logger::log_init(void)
{
    uint32_t i;

    m_pLogTypes = (struct log_type *)calloc(LOG_TYPE_FIRST_EXT_ID, sizeof(struct log_type));
    if (m_pLogTypes == NULL)
        return;

    // 注册日志类型
    for (i = 0; i < sizeof(reg_table) / sizeof(reg_table[0]); i++)
        log_register_internal(reg_table[i].log_type_name, reg_table[i].log_type_id);

    m_nLogTypesSize = LOG_TYPE_FIRST_EXT_ID;
}

// 日志释放
void Logger::log_free(void)
{
    uint32_t i;
    
    for (i = 0; i < sizeof(reg_table) / sizeof(reg_table[0]); i++) {
        uint32_t id = reg_table[i].log_type_id;
        if (m_pLogTypes[id].log_type_name)
            free((void*)(m_pLogTypes[id].log_type_name));
        m_pLogTypes[id].log_type_name = NULL;
    }

    if (m_pLogTypes) 
        free(m_pLogTypes);
    m_pLogTypes = NULL; 
}

int Logger::log_vlog(uint32_t level, uint32_t logtype, const char *format, va_list ap)
{
    int ret;
    char tbuf[64] = { 0 };

    if (level > m_nLevel)
        return 0;
    if (logtype >= m_nLogTypesSize)
        return -1;
    if (level > m_pLogTypes[logtype].log_type_level)
        return 0;
    
    time_t now;
    time(&now);
    tm *tm_now = localtime(&now);
    snprintf(tbuf, 64, "%04d.%02d.%02d %02d:%02d:%02d ",
                        tm_now->tm_year + 1900,
                        tm_now->tm_mon + 1,
                        tm_now->tm_mday,
                        tm_now->tm_hour,
                        tm_now->tm_min,
                        tm_now->tm_sec);
    fprintf(m_pFile, "%s", tbuf);
    ret = vfprintf(m_pFile, format, ap);
    fflush(m_pFile);
    return ret;
}
/**
 * @desc
 *  创建日志输出。
 * @param level
 *  日志等级
 * @param format
 *  格式化字符串
 * @return
 *  成功 0
 */
int Logger::log_out(uint32_t level, uint32_t logtype, const char *format, ...)
{
    va_list ap;
    int ret;

    va_start(ap, format);
    ret = log_vlog(level, logtype, format, ap);
    va_end(ap);
    return ret;
}
