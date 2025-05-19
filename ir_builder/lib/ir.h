#ifndef IR_H
#define IR_H

#include <stdio.h>

#include "tree_lib.h"
#include "ir_debug_macroses.h"
#include "operations.h"

const size_t START_IR_DATA_CAPACITY = 50;
const size_t OPERAND_NAME_LEN       = 50;
const size_t COMMENT_LEN            = 100;

const size_t MAIN_FUNC_NUM     = 777;
const size_t MAIN_FUNC_ARG_CNT = 1;

struct FuncLabel
{
    size_t num;
    size_t arg_cnt;
};

union Label
{
    size_t local;
    FuncLabel  func;
    const char *sys;
};

union IrOperandVal
{
    TreeElem_t num;
    size_t     tmp;
    size_t     var;
    size_t     arg;
    Label      label;
    // size_t     func_label;
    // size_t     local_label;
    size_t     sys_func;
    // IrOperationType   operation;    - этого вроде как нигде нет
};

struct IrOperand
{
    IrOperandType type;
    IrOperandVal  val;

    ON_IR_DEBUG(
    char comment_name[OPERAND_NAME_LEN];
    )
};

struct IrBlockTypeInfo
{
    const IrBlockType type;


    ON_IR_DEBUG(
    const char *const debug_name;
    )
};

struct IrOperationTypeInfo
{
    const IrOperationType type;

    ON_IR_DEBUG(
    const char *const debug_name;
    )
};


struct IrBlock
{
    // IrBlockType     block_type;
    // IrOperationType op_type;      

    const IrBlockTypeInfo *block_type_info;
    const IrOperationTypeInfo *operation_info;  // if block is not operation -> op_type = IR_OP_TYPE_INVALID
    
    IrOperand ret_operand;
    IrOperand operand_1; 
    IrOperand operand_2;

    Label label;

    ON_IR_DEBUG(
    char comment[COMMENT_LEN];
    )
};

struct ProperNamesCount
{
    size_t tmp_count;
    size_t var_count;
    size_t func_count;
    size_t local_label_count;
    size_t arg_count;
};

struct IR_struct
{
    IrBlock *blocks;

    size_t size;
    size_t capacity;

    ProperNamesCount proper_names_count; 
};

enum IrFuncRes
{
    IR_FUNC_OK,
    IR_FUNC_FAIL
};

#include "ir_debug.h"
#include "write_ir.h"

enum PrintIrBlockMode
{
    DEBUG_MODE,
    RELEASE_MODE
};



IrFuncRes IrCtor            (IR_struct *ir, const size_t start_capacity);
IrFuncRes IrDtor            (IR_struct *ir);
IrFuncRes IrRecalloc        (IR_struct *ir, const size_t new_capacity);
// IrFuncRes BuildIrByTreeNode (IR_struct *ir, Tree *code_tree, Node *cur_node);

IrOperand NodeToIrOperand(IR_struct *ir, const Node *const cur_node);

IrBlock *IrNewBlock(IR_struct *ir, const IrBlockType block_type, const IrOperationType operation_type, 
                    const IrOperand ret_operand, const IrOperand operand_1, const IrOperand operand_2, const Label label);


const IrBlockTypeInfo     *GetIrBlockTypeInfo     (IrBlockType     block_type);
const IrOperationTypeInfo *GetIrOperationTypeInfo (IrOperationType operation_type);

#define IS_LABEL_BLOCK_(block_)  ((block_)->block_type_info->type == IR_BLOCK_TYPE_FUNCTION_BODY || (block_)->block_type_info->type == IR_BLOCK_TYPE_LOCAL_LABEL)

#endif /*IR_H*/