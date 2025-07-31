#ifndef __RK_OS_LOG_H__
#define __RK_OS_LOG_H__

#define USER_COLOR_RESET            "\033[0m"
#define USER_COLOR_RED              "\033[31m"
#define USER_COLOR_GREEN            "\033[32m"
#define USER_COLOR_YALLOW           "\033[33m"
#define USER_COLOR_BLUE             "\033[34m"
#define USER_COLOR_MAGENTA          "\033[35m"
#define USER_COLOR_CYAN             "\033[36m"
#define USER_COLOR_WHITE            "\033[37m"

#define USER_LOG_NORMAL(fmt, ...)               printf(COLOR_WHITE fmt COLOR_RESET "\n", ##__VA_ARGS__)
#define USER_LOG_FAILTER(fmt, ...)              printf(COLOR_RED fmt COLOR_RESET "\n", ##__VA_ARGS__)
#define USER_LOG_INGO(fmt, ...)                 printf(COLOR_GREEN fmt COLOR_RESET "\n", ##__VA_ARGS__)
#define USER_LOG_WARN(fmt, ...)                 printf(COLOR_YELLOW fmt COLOR_RESET "\n", ##__VA_ARGS__)
#define USER_LOG_DEBUG(fmt, ...)                printf(COLOR_BLUE fmt COLOR_RESET "\n", ##__VA_ARGS__)
#define USER_LOG_ERROR(fmt, ...)                printf(COLOR_MAGENTA fmt COLOR_RESET "\n", ##__VA_ARGS__)
// 暂且未使用下面两个
#define PRINT_CYAN(fmt, ...)        printf(COLOR_CYAN fmt COLOR_RESET "\n", ##__VA_ARGS__)
#define PRINT_WHITE(fmt, ...)       printf(COLOR_WHITE fmt COLOR_RESET "\n", ##__VA_ARGS__)

#include <stdio.h>

typedef void (*rk_os_log_callback)(const char*, const char*, va_list);

#ifdef __cplusplus
extern "C" {
#endif

    void rk_os_log_trace(const char* tag, const char* msg, va_list list);
    void rk_os_log_debug(const char* tag, const char* msg, va_list list);
    void rk_os_log_info(const char* tag, const char* msg, va_list list);
    void rk_os_log_warn(const char* tag, const char* msg, va_list list);
    void rk_os_log_error(const char* tag, const char* msg, va_list list);
    void rk_os_log_fatal(const char* tag, const char* msg, va_list list);

#ifdef __cplusplus
}
#endif

// 【官方】设置中断输出log日志，通过"sudo tail -i /var/log/syslog"查看log
// 【修改】添加vfprientf，直接使其在终端输出。
class SysTerminalWrapper{
    public:
        SysTerminalWrapper();
        virtual ~SysTerminalWrapper();

    private:
        SysTerminalWrapper(const SysTerminalWrapper &);
        SysTerminalWrapper &operator=(const SysTerminalWrapper &);
};

#endif
