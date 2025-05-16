#include <stdio.h>

#include "text_color.h"
#include "logger.h"


void ConsoleColoredFprint(const char *const color, const char *const format, va_list args)
{
    lassert(color,   "color == NULL");
    lassert(format, "format == NULL");

    fprintf  (stderr, "\x1b[%sm", color);                  
    vfprintf (stderr, format, args);                        
    fprintf  (stderr, "\x1b[0m");                             
}


void HtmlColoredFprint(FILE *dest_file, const char *const color, const char *const format, va_list args)
{
    fprintf  (dest_file, DEFAULT_TAB);                       
    fprintf  (dest_file, "<span class=%s>", color);           
    vfprintf (dest_file, format, args);                       
    fprintf  (dest_file, "</span>\n");                             
}