#include "../include/logs.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

static LogType log_level = LOG_NONE;
static FILE* log_sink = NULL;

/* ============================================================================
 * Private function prototypes
 * ============================================================================
 */
static const char* basename_from_path(const char* path);

/* ============================================================================
 * Public API functions
 * ============================================================================
 */

int log_out(LogType log_type, const char* file, const int line, const char* func,
            const char* format, ...)
{
    if (!log_sink)
    {
        log_sink = stderr;
    }

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
        type_str = "ERROR  ";
        break;
    case LOG_WARNING:
        type_str = "WARNING";
        break;
    case LOG_INFO:
        type_str = "INFO   ";
        break;
    case LOG_DEBUG:
        type_str = "DEBUG  ";
        break;
    default:
        type_str = "LOG    ";
        break;
    }

    // Final output
    fprintf(log_sink, "[%s] [%s] [%s:%d %s] %s\n", time_str, type_str, basename_from_path(file),
            line, func, user_msg);

    return 0;
}

int set_log_level(LogType new_log_level)
{
    log_level = new_log_level;
    char* log_level_strs[6] = {"ALL", "DEBUG", "INFO", "WARNING", "ERORR", "NONE"};
    LOG_OUT(LOG_DEBUG, "Log level set to %s", log_level_strs[new_log_level]);
    return 0;
}

int set_log_sink(FILE* new_sink)
{
    if (new_sink)
        log_sink = new_sink;
    else
        log_sink = stderr;

    return 0;
}

/* ============================================================================
 * Private helper functions
 * ============================================================================
 */
static const char* basename_from_path(const char* path)
{
    if (path == NULL) // no path provided
        return "(null)";

    const char* base_name_win = strrchr(path, '\\');
    const char* base_name_unix = strrchr(path, '/');
    const char* base_name = NULL;

    if (base_name_unix == NULL && base_name_win == NULL) // no path delims
        return path;

    if (base_name_unix == NULL) // win delim only
        base_name = base_name_win;
    else if (base_name_win == NULL) // unix delim only
        base_name = base_name_unix;
    else // both delims (neither == NULL), pick the last one
        base_name = (base_name_unix > base_name_win) ? base_name_unix : base_name_win;

    return (base_name + 1); // return ptr to char after last delim
}