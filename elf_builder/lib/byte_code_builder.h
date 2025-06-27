#ifndef BYTE_CODE_BUILDER_H
#define BYTE_CODE_BUILDER_H

#include "elf_info.h"
#include "ir.h"

ElfFuncRes GetByteCodeFromIR(IR_struct *ir, TextSection *byte_code);
ElfFuncRes IrBlockToBytes(const IrBlock *block, TextSection *text);

const size_t START_PREAMBLE_LEN = 3;
const char START_PREAMBLE[START_PREAMBLE_LEN] = {0x48, 0x89, 0xe5};

const size_t EXIT_PREAMBLE_LEN = 14;
const char EXIT_PREAMBLE[EXIT_PREAMBLE_LEN] = { 0x48, 0x8b, 0x7D, 0x00,         // mov rdi, [rbp]
                                                0x48, 0x89, 0xEC,               // mov rsp, rbp
                                                0xB8, 0x3C, 0x00, 0x00, 0x00,   // mov eax, 0x60
                                                0x0F, 0x05 };                   // syscall

#endif