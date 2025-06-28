#ifndef BYTE_CODE_BUILDER_H
#define BYTE_CODE_BUILDER_H

#include "elf_info.h"
#include "ir.h"

ElfFuncRes GetByteCodeFromIR(IR_struct *ir, TextSection *byte_code);
ElfFuncRes IrBlockToBytes(const IrBlock *block, TextSection *text);

const size_t START_PREAMBLE_LEN = 3;
const char START_PREAMBLE[START_PREAMBLE_LEN] = {(char) 0x48, (char) 0x89, (char) 0xe5};

const size_t EXIT_PREAMBLE_LEN = 13;
const char EXIT_PREAMBLE[EXIT_PREAMBLE_LEN] = { (char) 0x48, (char) 0x89, (char) 0xC7,                              // mov rdi, rax
                                                (char) 0x48, (char) 0x89, (char) 0xEC,                              // mov rsp, rbp
                                                (char) 0xB8, (char) 0x3C, (char) 0x00, (char) 0x00, (char) 0x00,    // mov eax, 0x60
                                                (char) 0x0F, (char) 0x05 };                                         // syscall

// defined in makefile: OBJ_DIR_NAME        = \"$(OBJ_DIR)\"
//                      SYSTEM_LIB_BIN_NAME = \"$(SYSTEM_LIB_BIN_FILE)\"

#define SYSTEM_LIB_BIN_FULLNAME   OBJ_DIR_NAME "/" SYSTEM_LIB_BIN_NAME

const size_t SYS_PRINTF_SHIFT = 0x00;   // относительно начала файла system_lib.bin
const size_t SYS_SCANF_SHIFT  = 0x6D;
const size_t SYS_SQRT_SHIFT   = 0xB9;

const size_t SYS_PRINTF_NUM   = 990;    // идентификационный номер в массиве меток
const size_t SYS_SCANF_NUM    = 991;
const size_t SYS_SQRT_NUM     = 992;

const char *const SYS_PRINTF_NAME = "printf";
const char *const SYS_SCANF_NAME  = "scanf";


#endif