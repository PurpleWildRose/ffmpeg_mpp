#include <stdarg.h>
#include <syslog.h>

#include "rk_os_log.h"
#include "rk_os_env.h"

#define LINE_SZ 1024

static SysTerminalWrapper systerminal_wrapper;

SysTerminalWrapper::SysTerminalWrapper(){
    /*
    *   LOG_PID：在日志中包含进程 ID（便于多进程程序调试）。
    *   LOG_CONS：若日志无法发送到 syslog 守护进程，则直接输出到控制台。
    *   LOG_NDELAY：立即打开与 syslog 守护进程的连接（默认是首次调用 syslog() 时才打开）。
    *   LOG_ODELAY：延迟打开连接（默认行为）。
    *   LOG_PERROR：除了发送到 syslog，同时将日志输出到标准错误（stderr）。
    */
    int option = LOG_PID | LOG_CONS;
    RK_U32 syslog_perror = 0;

    os_get_env_u32("mpp_syslog_perror", &syslog_perror, 0);
    if (syslog_perror)
        option |= LOG_PERROR;

    // 初始化syslog日志
    openlog("mpp", option, LOG_USER);
}

SysTerminalWrapper::~SysTerminalWrapper()
{
    closelog();
}

void os_log_trace(const char* tag, const char* msg, va_list list)
{
    char line[LINE_SZ] = {0};
    snprintf(line, sizeof(line) - 1, "%s: %s", tag, msg);
    vsyslog(LOG_NOTICE, line, list);
}

void os_log_debug(const char* tag, const char* msg, va_list list)
{
    char line[LINE_SZ] = {0};
    snprintf(line, sizeof(line) - 1, "%s: %s", tag, msg);
    vsyslog(LOG_DEBUG, line, list);
}

void os_log_info(const char* tag, const char* msg, va_list list)
{
    char line[LINE_SZ] = {0};
    snprintf(line, sizeof(line) - 1, "%s: %s", tag, msg);
    vsyslog(LOG_INFO, line, list);
    vfprintf(stdout, line, list);

}

void os_log_warn(const char* tag, const char* msg, va_list list)
{
    char line[LINE_SZ] = {0};
    snprintf(line, sizeof(line) - 1, "%s: %s", tag, msg);
    vsyslog(LOG_WARNING, line, list);
    vfprintf(stdout, line, list);

}

void os_log_error(const char* tag, const char* msg, va_list list)
{
    char line[LINE_SZ] = {0};
    snprintf(line, sizeof(line) - 1, "%s: %s", tag, msg);
    vsyslog(LOG_ERR, line, list);
    vfprintf(stdout, line, list);

}

void os_log_fatal(const char* tag, const char* msg, va_list list)
{
    char line[LINE_SZ] = {0};
    snprintf(line, sizeof(line) - 1, "%s: %s", tag, msg);
    vsyslog(LOG_CRIT, line, list);
    vfprintf(stdout, line, list);

}




