#include <stdio.h>
#include <locale.h>

#include "front_reader.h"
#include "semantic_anal.h"

#include "tree_lib.h"
#include "tree_saver.h"


int main(const int argc, const char *argv[])
{
    fprintf(stderr, "FRONTEND START\n");

    FILE *input_file = GetInputCodeFile(argc, argv);

    Tree code = {};
    TreeCtor(&code, START_TREE_SIZE ON_TREE_DEBUG(, "expr_tree"));

    BuildTreeByCode(&code, input_file);

    fclose(input_file);

    FILE *output_file = GetOutputSaveFile(argc, argv);
    WriteTreeData(code.root_ptr, output_file);
    fclose(output_file);

    SemanticAnal(&code);

    TreeDtor(&code);

    fprintf(stderr, "FRONTEND END\n\n");
    return 0;
}