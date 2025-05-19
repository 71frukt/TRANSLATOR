#include <stdio.h>

#include "tree_lib.h"
#include "tree_saver.h"
#include "write_ir.h"
#include "logger.h"
#include "ir_logger_settings.h"
#include "ir.h"

// int main(const int argc, const char *argv[])
// {
//     fprintf(stderr, "\nIR_BUILDER_START\n\n");

//     logctor("ir_logfile.html", IR_LOGGER_SETTINGS_PREAMBLE);

//     FILE *input_ast = GetInputAST(argc, argv);

//     Tree code_tree = {};
//     TreeCtor(&code_tree, START_TREE_SIZE ON_TREE_DEBUG(, "code_tree"));

//     BuildTreeByFileData(input_ast, &code_tree);

//     IR_struct ir = {};
//     IrCtor(&ir, START_IR_DATA_CAPACITY);

//     NodeToIrOperand(&ir, code_tree.root_ptr);
//     IR_DUMP(&ir);

//     IrDtor(&ir);

//     TreeDtor(&code_tree);

//     fclose(input_ast);

//     fprintf(stderr, "IR_BUILDER_END\n\n");
// }


