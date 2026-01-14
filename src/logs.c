#include "../include/logs.h"

#include <stdarg.h>
#include <stdio.h>
#include <time.h>

static LogType log_level = LOG_NONE;
static FILE* log_sink = NULL;

int log_out(LogType log_type, const char* format, ...)
{
    // Only log messages at or above the current Log Level
    if (log_type < log_level)
        return 0;

    // Get timestamp
    char time_str[50];
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", t);

    // Format the user message
    char user_msg[512];
    va_list args;
    va_start(args, format);
    vsnprintf(user_msg, sizeof(user_msg), format, args);
    va_end(args);

    // Get string for Log type
    const char* type_str = "";
    switch (log_type)
    {
    case LOG_ERROR:
        type_str = "ERROR";
        break;
    case LOG_WARNING:
        type_str = "WARNING";
        break;
    case LOG_INFO:
        type_str = "INFO";
        break;
    case LOG_DEBUG:
        type_str = "DEBUG";
        break;
    default:
        type_str = "LOG";
        break;
    }

    // Final output
    fprintf(log_sink, "[%s] [%s] %s\n", time_str, type_str, user_msg);

    return 0;
}

int set_log_level(LogType new_log_level)
{
    log_level = new_log_level;
    char* log_level_strs[6] = {"ALL", "DEBUG", "INFO", "WARNING", "ERORR", "NONE"};
    log_out(LOG_DEBUG, "Log level set to %s", log_level_strs[new_log_level]);
    return 0;
}

int set_log_sink(FILE* new_sink)
{
    log_sink = new_sink;
    return 0;
}
