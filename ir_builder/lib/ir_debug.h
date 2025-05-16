#ifndef IR_DEBUG_H
#define IR_DEBUG_H

#include "ir.h"
#include "logger.h"


const size_t ERROR_STR_MAX_SIZE = 300;

enum IrCondition
{
    IR_OK           = 1 << 0,
    IR_PTR_ERR      = 1 << 1,
    IR_DATA_PTR_ERR = 1 << 2,
    IR_OVERFLOW     = 1 << 3
};


char *GetIrErrors(int error);

int IrVerify(const IR_struct *ir);

void IrDump (const IR_struct *ir);
void BlocksDump_tables (const IR_struct *ir);

#define IR_ERROR_HANDLE(ir_func) do                                                                                         \
{                                                                                                                           \
    IrFuncRes func_res = ir_func;                                                                                           \
    if (func_res != IR_FUNC_OK)                                                                                             \
    {                                                                                                                       \
        log(ERROR, "Failed " #ir_func);                                                                                     \
    }                                                                                                                       \
} while (0)


#endif /*IR_DEBUG_H*/