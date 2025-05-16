#ifndef CONSOLE_COLOR_H
#define CONSOLE_COLOR_H

#define CONSOLE_BLACK   "30" 
#define CONSOLE_RED     "31"
#define CONSOLE_GREEN   "32"
#define CONSOLE_YELLOW  "33"
#define CONSOLE_BLUE    "34"
#define CONSOLE_MAGENTA "35"
#define CONSOLE_CYAN    "36"
#define CONSOLE_WHITE   "37"

#define HTML_BLUE    "blue"
#define HTML_CYAN    "cyan"
#define HTML_SKYBLUE "SkyBlue"
#define HTML_RED     "red"
#define HTML_GREEN   "green"
#define HTML_YELLOW  "yellow"
#define HTML_WHITE   "white"
#define HTML_MAGENTA "Fuchsia"

#define CONSOLE_COLORED(STR, COLOR) "\x1b[" COLOR "m" STR "\x1b[0m"

#define HTML_COLORED(STR, color)  "<span class=" color ">" STR "</span>"


void ConsoleColoredFprint(const char *const color, const char *const format, va_list args);
void HtmlColoredFprint(FILE *dest_file, const char *const color, const char *const format, va_list args);

#endif