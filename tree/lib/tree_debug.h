#ifndef TREE_DEBUG_H
#define TREE_DEBUG_H

#include "tree.h"

#ifdef TREE_DEBUG
#define ON_TREE_DEBUG(...)  __VA_ARGS__
#define TREE_DUMP(tree)  TreeDump(tree, __FILE__, __LINE__, __func__)

#else
#define ON_TREE_DEBUG(...)
#define TREE_DUMP(tree)
#endif

// #define BUILD_FOLDER_NAME     "build"
// #define LOGS_FOLDER_NAME      BUILD_FOLDER_NAME "/" "logs"
// #define GRAPH_FOLDER_NAME     LOGS_FOLDER_NAME "/" "graphs"

#define TREE_LOGFILE_NAME     "tree_logfile.html"
const int PATH_NAME_LEN   = 100;

FILE *OpenTreeLogFile  ();
void  CloseTreeLogFile ();
char *GetFilePath  (const char *name, const char *folder, char *path);
void  TreeDump     (Tree *tree, const char *file, int line, const char *func);

#endif