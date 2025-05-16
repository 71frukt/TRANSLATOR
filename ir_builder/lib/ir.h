#ifndef IR_H
#define IR_H

#include <stdio.h>

#include "libpyam_ir.h"
#include "tree_lib.h"
#include "ir_debug_macroses.h"

const size_t START_IR_DATA_CAPACITY = 50;

struct FuncLabel
{
    size_t num;
    size_t arg_cnt;
};

union Label
{
    size_t local;
    FuncLabel func;
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
};

struct IrBlockTypeInfo
{
    IrBlockType type;

    const char *const pyam_ir_name;

    ON_IR_DEBUG(
    const char *const debug_name;
    )
};

struct IrOperationTypeInfo
{
    IrOperationType type;

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
};

struct ProperNamesCount
{
    size_t tmp_count;
    size_t var_count;
    size_t func_count;
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


// const size_t IR_OP_BLOCK_TYPES_COUNT = 9;

// const IrBlockTypeInfo IR_BLOCK_TYPES_INFO[IR_OP_BLOCK_TYPES_COUNT] =         // use assert for cmp IrBlockType getting array elem!
// {
//     { IR_BLOCK_TYPE_CALL_FUNCTION  ON_IR_DEBUG( , "call_func"     ) },
//     { IR_BLOCK_TYPE_FUNCTION_BODY  ON_IR_DEBUG( , "func"          ) },
//     { IR_BLOCK_TYPE_COND_JUMP      ON_IR_DEBUG( , "cond_jump"     ) },
//     { IR_BLOCK_TYPE_ASSIGNMENT     ON_IR_DEBUG( , "assignment"    ) },
//     { IR_BLOCK_TYPE_OPERATION      ON_IR_DEBUG( , "operation"     ) },
//     { IR_BLOCK_TYPE_RETURN         ON_IR_DEBUG( , "return"        ) },
//     { IR_BLOCK_TYPE_LOCAL_LABEL    ON_IR_DEBUG( , "local_label"   ) },
//     { IR_BLOCK_TYPE_SYSCALL        ON_IR_DEBUG( , "syscall"       ) },
//     { IR_BLOCK_TYPE_INVALID        ON_IR_DEBUG( , "block_invalid" ) },
// };




// const size_t IR_OPERATIONS_COUNT = 12;

// const IrOperationTypeInfo IR_OPERATIONS_INFO[IR_OPERATIONS_COUNT] =         // use assert for cmp IrBlockType getting array elem!
// {
//     { IR_OPERATION_TYPE_SUM                ON_IR_DEBUG( , "+"          ) },            // ON_IR_DEBUG( , "op_sum"     ) },
//     { IR_OPERATION_TYPE_SUB                ON_IR_DEBUG( , "-"          ) },            // ON_IR_DEBUG( , "op_sub"     ) },
//     { IR_OPERATION_TYPE_MUL                ON_IR_DEBUG( , "*"          ) },            // ON_IR_DEBUG( , "op_mul"     ) },
//     { IR_OPERATION_TYPE_DIV                ON_IR_DEBUG( , "/"          ) },            // ON_IR_DEBUG( , "op_div"     ) },
//     { IR_OPERATION_TYPE_POW                ON_IR_DEBUG( , "^"          ) },            // ON_IR_DEBUG( , "op_pow"     ) },
//     { IR_OPERATION_TYPE_EQ                 ON_IR_DEBUG( , "=="         ) },            // ON_IR_DEBUG( , "op_eq"      ) },
//     { IR_OPERATION_TYPE_NEQ                ON_IR_DEBUG( , "!="         ) },            // ON_IR_DEBUG( , "op_neq"     ) },
//     { IR_OPERATION_TYPE_LESS               ON_IR_DEBUG( , "<"          ) },            // ON_IR_DEBUG( , "op_less"    ) },
//     { IR_OPERATION_TYPE_LESSEQ             ON_IR_DEBUG( , "<="         ) },            // ON_IR_DEBUG( , "op_lesseq"  ) },
//     { IR_OPERATION_TYPE_GREAT              ON_IR_DEBUG( , ">"          ) },            // ON_IR_DEBUG( , "op_great"   ) },
//     { IR_OPERATION_TYPE_GREATEQ            ON_IR_DEBUG( , ">="         ) },            // ON_IR_DEBUG( , "op_greateq" ) },
//     { IR_OPERATION_TYPE_NONE  ON_IR_DEBUG( , "op_invalid" ) }             // ON_IR_DEBUG( , "op_INVALID" ) }
// };



IrFuncRes IrCtor            (IR_struct *ir, size_t start_capacity);
IrFuncRes IrDtor            (IR_struct *ir);
IrFuncRes IrRecalloc        (IR_struct *ir, size_t new_capacity);
// IrFuncRes BuildIrByTreeNode (IR_struct *ir, Tree *code_tree, Node *cur_node);

IrOperand GetOperand(IR_struct *ir, Tree *code_tree, Node *cur_node);

IrBlock *IrNewBlock(IR_struct *ir, const IrBlockType block_type, const IrOperationType operation_type, 
    const IrOperand operand_1, const IrOperand operand_2, const IrOperand ret_operand);

IrBlock *IrBuildAssignBlock(IR_struct *ir, IrOperand operand_1, IrOperand operand_2 /*, IrOperand ret_operand*/);

const IrBlockTypeInfo *GetIrBlockTypeInfo (IrBlockType block_type);
const IrOperationTypeInfo *GetIrOperationTypeInfo (IrOperationType operation_type);


#endif /*IR_H*/