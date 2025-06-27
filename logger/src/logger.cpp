#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>

#include "logger.h"
#include "text_color.h"

FILE   *LogFile = NULL;
size_t  DefaultTabNum = 1;

FILE *OpenLog(const char *const logfile_name, const char *const style_preamble)
{
    LogFile = fopen(GetFileFullPath(logfile_name, LOGS_FOLDER_NAME), "w");
    assert(LogFile && "Error when opening LogFile");

    if (LogFile == NULL)
        fprintf(stderr, "Error when opening LogFile\n");

    fprintf(LogFile, "<html>                                                                                                                    \n"
                     "\t<head>                                                                                                                  \n"
                     "\t<title>%s</title>                                                                                                       \n"
                     "\t<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/4.5.0/css/bootstrap.min.css\">                \n"
                     "\t</head>                                                                                                                 \n"
                     "\t<body>                                                                                                                  \n"
                     "\t<div class=\"jumbotron text-center\">                                                                                   \n"
                     "\t\t<h1>%s</h1>                                                                                                           \n"
                     "\t</div>                                                                                                                  \n"
                     "\t<pre>                                                                                                                   \n"
                     "\t<style>                                                                                                                 \n"
                     "%s                                                                                                                        \n"
                     "\t</style>                                                                                                                \n"
                    , logfile_name, logfile_name, style_preamble);


    atexit(CloseLogFile);

    return LogFile;
}

void CloseLogFile()
{
    fprintf(LogFile,    "\t\t</pre>     \n"
                        "\t</body>      \n"
                        "</html>");

    assert(fclose(LogFile) == 0 && "Error when closing LogFile");
}

//     log(ERROR, "HASH_TABLE_VERIFY failed! Errors:\n %s\n", "ERROR_TEST!"); 

void LogPrint(const char *const file, const int line, const char *const func, LogType log_type, const char *const format, ...)
{   
    assert(file);
    assert(func);
    assert(format);
    assert(LogFile && "LogFile = NULL, (!!!) maybe forgot logctor?");

    va_list args_console;
    va_list args_file;

    va_start(args_console, format);
    va_start(args_file,    format);

    for (size_t i = 0; i < DefaultTabNum; i++)
        ON_LOGS(fprintf(LogFile, LOGGER_DEFAULT_TAB));

    switch (log_type)
    {
    case LOG:
        ON_LOGS(
        vfprintf(LogFile, format, args_file);
        );
        break;
        
    case INFO:
        ON_LOGS(
        fprintf(LogFile, HTML_COLORED("info:\t", HTML_CYAN));
        HtmlColoredFprint(LogFile, HTML_WHITE, format, args_file);
        );

        fprintf(stderr, CONSOLE_COLORED("info:\t", CONSOLE_CYAN));
        ConsoleColoredFprint(CONSOLE_WHITE, format, args_console);
        break;

    case DUMP:
        ON_LOGS(
        fprintf(LogFile,  HTML_COLORED("Dumped", HTML_SKYBLUE)  " from "  HTML_COLORED("%s:%d (%s)\n", HTML_CYAN)   "\n", file, line, func);
        HtmlColoredFprint(LogFile, HTML_SKYBLUE, format, args_file);
        );
        break;
        
    case ASSERT:
        ON_LOGS(
        fprintf(LogFile, HTML_COLORED("Assertation failed", HTML_MAGENTA) " in " HTML_COLORED("%s:%d(%s)", HTML_CYAN) "\n", file, line, func);
        HtmlColoredFprint(LogFile, HTML_MAGENTA, format, args_file);
        );

        fprintf(stderr, CONSOLE_COLORED("Assertation failed", CONSOLE_MAGENTA) " in " CONSOLE_COLORED("%s:%d(%s)", CONSOLE_CYAN) "\n", file, line, func);
        ConsoleColoredFprint(CONSOLE_MAGENTA, format, args_console);
        break;
        
    case ERROR:
        ON_LOGS(
            fprintf(LogFile, HTML_COLORED("Error", HTML_RED) " in " HTML_COLORED("%s:%d(%s)", HTML_CYAN) "\n", file, line, func);
            HtmlColoredFprint(LogFile, HTML_RED, format, args_file);
        );
        
        fprintf(stderr, CONSOLE_COLORED("Error", CONSOLE_RED) " in " CONSOLE_COLORED("%s:%d(%s)", CONSOLE_CYAN) "\n", file, line, func);
        ConsoleColoredFprint(CONSOLE_RED, format, args_console);
        break;

    case WARNING:
        ON_LOGS(
        fprintf(LogFile, HTML_COLORED("Warning", HTML_YELLOW) " in " HTML_COLORED("%s:%d(%s)", HTML_CYAN) "\n", file, line, func);
        HtmlColoredFprint(LogFile, HTML_YELLOW, format, args_file);
        );

        fprintf(stderr, CONSOLE_COLORED("Warning", CONSOLE_YELLOW) " in " CONSOLE_COLORED("%s:%d(%s)", CONSOLE_CYAN) "\n", file, line, func);
        ConsoleColoredFprint(CONSOLE_YELLOW, format, args_console);
        break;

    default:
        break;
    }

    if (log_type == ERROR || log_type == WARNING || log_type == ASSERT || log_type == INFO)
    {
        fprintf(stderr,  "\n");
        fprintf(LogFile, "\n");
    }


    fflush(LogFile);
    va_end(args_console);
    va_end(args_file);
}

const char *GetFileFullPath(const char *const file_name, const char *const path)
{
    static char full_path[PATH_MAX_LEN] = {};

    size_t file_name_len = strlen(file_name);
    size_t path_len      = strlen(path);

    if (file_name_len + path_len >= PATH_MAX_LEN - 2)
        log(ERROR, "PATH_MAX_LEN overflow");

    snprintf(full_path, PATH_MAX_LEN, "%s/%s", path, file_name);

    return full_path;
}