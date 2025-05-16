#include <stdio.h>

#include "back_writer.h"
#include "tree_lib.h"
#include "tree_saver.h"

int main(const int argc, const char *argv[])
{
    fprintf(stderr, "BACKEND_START\n");

    FILE *source_file = GetInputSaveFile(argc, argv);

    Tree source_tree = {};
    TreeCtor(&source_tree, START_TREE_SIZE ON_TREE_DEBUG(, "source"));

    BuildTreeByFileData(source_file, &source_tree);

    TREE_DUMP(&source_tree);

    FILE *asm_file = GetOutputAsmFile(argc, argv);

    PrintStartRegisterValues(asm_file);

    PrintAsmCodeByNode(source_tree.root_ptr, asm_file);

    TreeDtor(&source_tree);
    
    fprintf(stderr, "BACKEND_END\n");

    return 0;
}