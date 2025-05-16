#include <stdio.h>

#include "reverse_frontend_info.h"
#include "../../tree_saver/tree_saver.h"

int main(const int argc, const char *argv[])
{
    fprintf(stderr, "START REVERSE_FRONTEND\n\n");

    FILE *source_file = GetInputSaveFile(argc, argv);

    Tree source_tree = {};
    TreeCtor(&source_tree, START_TREE_SIZE ON_TREE_DEBUG(, "source_tree"));

    BuildTreeByFileData(source_file, &source_tree);
    fclose(source_file);

    FILE *refrontanded_file = fopen(REFRONT_FILE_NAME, "w");

    PrintNodeMySym(source_tree.root_ptr, refrontanded_file);

    fprintf(stderr, "\n\nEND REVERSE_FRONTEND\n\n");    
}