#ifndef IR_LOGGER_SETTINGS_H
#define IR_LOGGER_SETTINGS_H

#include "text_color.h"     // it is logger header

#define COLORS_HTML_PREAMBLE                                                                                                       \
    "\tbody { background: linear-gradient(to right, #0f2027, #203a43, #2c5364);}                                         \n" \
    "\tpre { color: #ffffff; }                                                                                               \n" \
    "\t." HTML_RED     "{ color:" HTML_RED     ";}                                                                             \n" \
    "\t." HTML_YELLOW  "{ color:" HTML_YELLOW  ";}                                                                             \n" \
    "\t." HTML_BLUE    "{ color:" HTML_BLUE    ";}                                                                             \n" \
    "\t." HTML_GREEN   "{ color:" HTML_GREEN   ";}                                                                             \n" \
    "\t." HTML_CYAN    "{ color:" HTML_CYAN    ";}                                                                             \n" \
    "\t." HTML_MAGENTA "{ color:" HTML_MAGENTA ";}                                                                             \n" \
    "\t." HTML_SKYBLUE "{ color:" HTML_SKYBLUE ";}                                                                             \n" \
    "\t.jumbotron {                                                                                                            \n" \
    "\t\tbackground: linear-gradient(to bottom, #ffffff,  #ffffff, #001aff, #001aff, #b00000, #b00000);            \n" \
    "\t\tcolor: white;                                                                                                         \n" \
    "\t\ttext-shadow: 1px 1px 2px rgba(0,0,0,0.5);                                                                           \n" \
    "\t\tborder-radius: 0;                                                                                                     \n" \
    "\t\tmargin-bottom: 0;                                                                                                     \n" \
    "\t}                                                                                                                       \n" \
                     


#define TABLE_HTML_COLOR                                            \
    "\t.tables-row {                                            \n" \
    "\t    margin-left: 100px;                                  \n" \
    "\t    display: flex;                                       \n" \
    "\t    gap: 20px;                                           \n" \
    "\t}                                                        \n" \
    "\t                                                         \n" \
    "\t.table-with-caption {                                    \n" \
    "\t    display: flex;                                       \n" \
    "\t    flex-direction: column;                              \n" \
    "\t    align-items: center;                                 \n" \
    "\t}                                                        \n" \
    "\t                                                         \n" \
    "\t.table-caption {                                         \n" \
    "\t    margin-bottom: 10px;                                 \n" \
    "\t    font-weight: bold;                                   \n" \
    "\t}                                                        \n" \
    "\t.table {                                                 \n" \
    "\t    border-collapse: collapse;                           \n" \
    "\t    width: 100px;                                        \n" \
    "\t}                                                        \n" \
    "\t                                                         \n" \
    "\t.table td {                                              \n" \
    "\t    color: #00ccff;                                    \n" \
    "\t    border: 1px solid #3a4a5a;                         \n" \
    "\t    height: 35px;                                        \n" \
    "\t    text-align: center;                                  \n" \
    "\t}                                                        \n" \
    "\t                                                         \n" \
    "\ttable tr:first-child td {                                \n" \
    "\t    color: #ffffff;                                    \n" \
    "\t    font-weight: bold;                                   \n" \
    "\t    background-color: rgba(0,0,0,0.2);                 \n" \
    "\t}                                                        \n" \
    "\t                                                         \n" \
    "\ttable td:first-child {                                   \n" \
    "\t    color: #ffffff;                                    \n" \
    "\t}                                                        \n"


#define IR_LOGGER_SETTINGS_PREAMBLE  COLORS_HTML_PREAMBLE TABLE_HTML_COLOR

#endif /*IR_LOGGER_SETTINGS_H*/