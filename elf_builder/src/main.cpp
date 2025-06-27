#include <stdio.h>

#include "elf_info.h"
#include "elf_debug.h"
#include "elf_logger_settings.h"
#include "logger.h"

#include "instructions_to_bytes.h"

// BASE_OUTPUT_ELF_NAME  - defined in makefile

int main()
{
    logctor("elf_builder_logger", ELF_LOGGER_SETTINGS_H);
    
    fprintf(stderr, "\nELF_BUILDER_START\n\n");


    FILE *dest_elf = fopen(BASE_OUTPUT_ELF_NAME, "wb");

    ElfData elf_data = {};
    ELF_HANDLE(ElfDataFill(&elf_data));
    
    // ELF_HANDLE(WriteCode(dest_elf, &elf_data));

    TextSection text = {};


    // MovRegReg(&text, RDI_CODE, RBX_CODE);
    // MovRegImm(&text, RSP_CODE, 12345678910);

    MovRegMem(&text, RAX_CODE, RBP_CODE, -1024);
    for (size_t i = 0; i < text.size; i++)
        fprintf(stderr, "%02x ", (unsigned char) text.code[i]);



    fclose(dest_elf);

    fprintf(stderr, "\nELF_BUILDER_END\n\n");
}


