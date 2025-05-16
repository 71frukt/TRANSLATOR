#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

#include "stdlib.h"

#define DEFAULT_LOGFILE_NAME  "LogFile.html"
// #define LOGS_FOLDER_NAME            "logs"

#define DEFAULT_TAB           "\t"

enum LogType
{
    LOG,
    INFO,
    DUMP,
    ASSERT,
    ERROR,
    WARNING
};

FILE *OpenLog(const char *const logfile_name, const char *const style_preamble);

void  CloseLogFile();
void  LogPrint(const char *const file, const int line, const char *const func, LogType log_type, const char *const format, ...);

const size_t PATH_MAX_LEN = 100;
const char *GetFileFullPath(const char *const file_name, const char *const path);           // ret string is static array!

#define log(log_type, format, ...)  LogPrint(__FILE__, __LINE__, __func__, log_type, format, ##__VA_ARGS__)

#ifdef USE_LOGS

#define lassert(condition, fall_text)                           \
do                                                              \
{                                                               \
    if (!(condition))                                           \
    {                                                           \
        log(ASSERT, fall_text);                                 \
        abort();                                                \
    }                                                           \
} while(0)


#define ON_LOGS(...)  __VA_ARGS__

#define logctor(logfile_name, ...)                                          \
do                                                                          \
{                                                                           \
    if (#logfile_name[0] == '\0')                                           \
    {                                                                       \
        system("mkdir -p " LOGS_FOLDER_NAME);                               \
        OpenLog(LOGS_FOLDER_NAME "/" DEFAULT_LOGFILE_NAME, __VA_ARGS__ ""); \
    }                                                                       \
                                                                            \
    else                                                                    \
        OpenLog(logfile_name "",  __VA_ARGS__ "");                          \
} while(0)


#else
#define lassert(condition, fall_text)
// #define log(log_type, format, ...)
#define ON_LOGS(...)  
#define logctor(logfile_name, ...)
#endif

#endif