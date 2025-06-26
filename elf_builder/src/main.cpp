#include <stdio.h>

#include "elf_info.h"
#include "elf_debug.h"
#include "elf_logger_settings.h"
#include "logger.h"

// BASE_OUTPUT_ELF_NAME  - defined in makefile

int main()
{
    logctor("elf_builder_logger", ELF_LOGGER_SETTINGS_H);
    
    fprintf(stderr, "\nELF_BUILDER_START\n\n");


    FILE *dest_elf = fopen(BASE_OUTPUT_ELF_NAME, "wb");

    ElfData elf_data = {};
    ELF_HANDLE(ElfDataCtor(&elf_data));
    
    ELF_HANDLE(WriteCode(dest_elf, &elf_data));

    fclose(dest_elf);
    ELF_HANDLE(ElfDataDtor(&elf_data));


    fprintf(stderr, "ELF_BUILDER_END\n\n");
}


