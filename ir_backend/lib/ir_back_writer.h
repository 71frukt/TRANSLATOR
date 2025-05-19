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

#define IR_BACK_HANDLER(ir_back_func_) do                                                                                   \
{                                                                                                                            \
    IrBackendFuncRes func_res = ir_back_func_;                                                                              \
    if (func_res != IR_BACK_FUNC_OK)                                                                                              \
    {                                                                                                                           \
        log(ERROR, "Failed " #ir_back_func_);                                                                                  \
    }                                                                                                                             \
} while (0)


#endif /*IR_BACK_WRITER_H*/