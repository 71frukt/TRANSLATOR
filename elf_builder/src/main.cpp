#include <stdio.h>

#include "ir.h"
#include "tree_saver.h"
#include "elf_info.h"
#include "elf_debug.h"
#include "elf_logger_settings.h"
#include "logger.h"

#include "instructions_to_bytes.h"

// BASE_OUTPUT_ELF_NAME  - defined in makefile

int main(const int argc, const char *argv[])
{
    fprintf(stderr, "\nELF_BUILDER_START\n\n");
    
    logctor("elf_builder_logger.html", ELF_LOGGER_SETTINGS_PREAMBLE);

    FILE *dest_elf = fopen(BASE_OUTPUT_ELF_NAME, "wb");
    ElfData elf_data = {};
    
    FILE *input_ast   = GetInputAST(argc, argv);
    
    Tree code_tree = {};
    TreeCtor(&code_tree, START_TREE_SIZE ON_TREE_DEBUG(, "code_tree"));
    
    BuildTreeByFileData(input_ast, &code_tree);
    
    IR_struct ir = {};
    IrCtor(&ir, START_IR_DATA_CAPACITY);
    
    NodeToIrOperand(&ir, code_tree.root_ptr);
    IR_DUMP(&ir);
    
    ELF_HANDLE(ElfDataFill(&elf_data, &ir));

    IrDtor(&ir);
    TreeDtor(&code_tree);
    fclose(input_ast);

    ELF_HANDLE(WriteCode(dest_elf, &elf_data));
    


    // MovRegReg(&text, RDI_CODE, RBX_CODE);
    // MovRegImm(&text, RSP_CODE, 12345678910);
    // SubRegImm(&text, RSP_CODE, 8);
    // fprintf(stderr, "\n\nmycode\n");
    // TextSection text = {};
    // MovRegImm(&text, RAX_CODE, 0x60);
    // for (size_t i = 0; i < text.size; i++)
    //     fprintf(stderr, "%02x ", (unsigned char) text.code[i]);



    fclose(dest_elf);

    fprintf(stderr, "\nELF_BUILDER_END\n\n");
}


