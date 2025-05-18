#ifndef IR_BACK_WRITER_H
#define IR_BACK_WRITER_H

#include <stdio.h>

#include "ir.h"

enum IrBackendFuncRes
{
    IR_BACK_FUNC_OK   ,
    IR_BACK_FUNC_FAIL ,
};

FILE *GetOutputAsmFile(const int argc, const char *argv[]);

IrBackendFuncRes IrToAsmCode      (const IR_struct *const ir  , FILE *dest_file);



#endif /*IR_BACK_WRITER_H*/