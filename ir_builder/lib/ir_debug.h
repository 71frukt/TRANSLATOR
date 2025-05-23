#ifndef IR_DEBUG_H
#define IR_DEBUG_H

#include <string.h>

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

void LoadOperandRealName(IrOperand *named_operand, const Node *named_node);
void MakeBlockComment(IrBlock *block);


#define AddBlockComment(block_, ...)  snprintf(block_->comment, COMMENT_LEN, __VA_ARGS__);
#endif /*IR_DEBUG_H*/