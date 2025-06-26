#ifndef BYTE_CODE_BUILDER_H
#define BYTE_CODE_BUILDER_H

#include "elf_info.h"
#include "ir.h"

ElfFuncRes GetByteCodeFromIR(IR_struct *ir, ByteCode *byte_code);


#endif