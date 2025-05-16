#include <stdio.h>

#include "tree_lib.h"

// #define BASE_SAVE_TREE_FILE_NAME  "..." - defined in MakeFile

FILE *GetInputSaveFile   (const int argc, const char *argv[]);
FILE *GetOutputSaveFile  (const int argc, const char *argv[]);

void WriteTreeData       (Node *cur_node, FILE *dest_file);
void BuildTreeByFileData (FILE *source, Tree *tree);
Node *MakeAndLinkNodes   (FILE *source, Tree *tree);