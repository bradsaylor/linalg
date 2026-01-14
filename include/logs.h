#ifndef LOGS_H
#define LOGS_H

#define LOG_OUT(log_type, format, ...)                                                             \
    do                                                                                             \
    {                                                                                              \
        log_out(log_type, __FILE__, __LINE__, __func__, format, ##__VA_ARGS__);                    \
    }; while (0)

typedef enum
{
    LOG_ALL,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_NONE
} LogType;

extern LogType log_level;

int log_out(LogType log_type, const char* file, const int line, const char* func,
            const char* format, ...);

int set_log_level(LogType log_level);

int set_log_sink(FILE* new_sink);

#endif // LOGS_H
