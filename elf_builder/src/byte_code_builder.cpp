#include <stdio.h>

#include "elf_info.h"
#include "ir.h"
#include "byte_code_builder.h"

ElfFuncRes GetByteCodeFromIR(IR_struct *ir, TextSection *text_sect)
{
    // FIXME: заменить эту хуйню на построение реального кода

    text_sect->size = 12;
    text_sect->code = (char *) calloc(12, sizeof(char));

    char my_code[12] = 
    {
        0xb8, 0x3c, 0x00, 0x00, 0x00,  // mov eax, 60 (syscall exit)
        0xbf, 0x7b, 0x00, 0x00, 0x00,  // mov edi, 666 (аргумент)
        0x0f, 0x05                     // syscall
    };


    for (size_t i = 0; i < 12; i++)
    {
        text_sect->code[i] = my_code[i];
    }

    return ELF_FUNC_OK;
}