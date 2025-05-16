#ifndef WRITE_IR_H
#define WRITE_IR_H

#include "ir.h"
// #define BASE_UNPUT_AST_FILE_NAME = "..."  - defined in MakeFile


FILE *GetInputAST(const int argc, const char *argv[]);
IrFuncRes PrintIrBlock(IrBlock *block, FILE *dest_file);
const char *GetOperandName (const IrOperand operand);       // res str is static array

// IrFuncRes PrintIr(IR_struct *ir, FILE *file);

// #define PRINT_IR()

#endif /*WRITE_IR_H*/