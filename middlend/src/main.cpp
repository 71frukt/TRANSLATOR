#include <stdio.h>

#include "tree_saver.h"
#include "simplifier.h"

int main(const int argc, const char *argv[])
{
    fprintf(stderr, "START MIDDLEND\n\n");

    FILE *source_file = GetInputSaveFile(argc, argv);

    Tree code_tree = {};
    TreeCtor(&code_tree, START_TREE_SIZE ON_TREE_DEBUG(, "code_tree"));
    
    BuildTreeByFileData(source_file, &code_tree);
    fclose(source_file);

    SimplifyTree(&code_tree);

    FILE *dest_file = GetOutputSaveFile(argc, argv);
    WriteTreeData(code_tree.root_ptr, dest_file);
    fclose(dest_file);

    TreeDtor(&code_tree);
    fprintf(stderr, "\n\nEND MIDDLEND\n\n");

    return 0;
}