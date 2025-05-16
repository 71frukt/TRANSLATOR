#include <stdio.h>

#include "ir.h"

static void SetPoisonOperationBlock(IrBlock *operation_block);
static IrOperationType TreeToIrMathOp(MathOperation_enum cur_tree_math_op);


// ====================== BLOCKS TYPE INFO ===============================
#define INIT_BLOCK_TYPE_INFO_(block_type_, pyam_ir_name, debug_name_)   \
    static const IrBlockTypeInfo struct_info_##block_type_ = {block_type_, pyam_ir_name  ON_IR_DEBUG( , debug_name_)}

                        //      block_type               pyam_ir_name    debug_name
    INIT_BLOCK_TYPE_INFO_( IR_BLOCK_TYPE_CALL_FUNCTION ,  "RingRing"   , "call_func"     );    
    INIT_BLOCK_TYPE_INFO_( IR_BLOCK_TYPE_FUNCTION_BODY ,  "Gyat"       , "func"          );
    INIT_BLOCK_TYPE_INFO_( IR_BLOCK_TYPE_COND_JUMP     ,  "Frog"       , "cond_jump"     );    
    INIT_BLOCK_TYPE_INFO_( IR_BLOCK_TYPE_ASSIGNMENT    ,  "Gnoming"    , "assignment"    );
    INIT_BLOCK_TYPE_INFO_( IR_BLOCK_TYPE_OPERATION     ,  "Digging"    , "operation"     );
    INIT_BLOCK_TYPE_INFO_( IR_BLOCK_TYPE_RETURN        ,  "Viperr"     , "return"        );
    INIT_BLOCK_TYPE_INFO_( IR_BLOCK_TYPE_LOCAL_LABEL   ,  "Cherepovec" , "local_label"   );
    INIT_BLOCK_TYPE_INFO_( IR_BLOCK_TYPE_SYSCALL       ,  "Bobb"       , "syscall"       );
    INIT_BLOCK_TYPE_INFO_( IR_BLOCK_TYPE_INVALID       ,  "Gg"         , "block_invalid" );

#undef INIT_BLOCK_TYPE_INFO_
// ========================================================================


// ====================== OPERATIONS TYPE INFO ============================
#define INIT_OPERATION_INFO_(operation_type_, debug_name_)   \
    static const IrOperationTypeInfo struct_info_##operation_type_ = {operation_type_ ON_IR_DEBUG( , debug_name_)}

                        //    operation_type         debug_name
    INIT_OPERATION_INFO_( IR_OPERATION_TYPE_SUM     , "+"          );
    INIT_OPERATION_INFO_( IR_OPERATION_TYPE_SUB     , "-"          );
    INIT_OPERATION_INFO_( IR_OPERATION_TYPE_MUL     , "*"          );
    INIT_OPERATION_INFO_( IR_OPERATION_TYPE_DIV     , "/"          );
    INIT_OPERATION_INFO_( IR_OPERATION_TYPE_POW     , "^"          );
    INIT_OPERATION_INFO_( IR_OPERATION_TYPE_EQ      , "=="         );
    INIT_OPERATION_INFO_( IR_OPERATION_TYPE_NEQ     , "!="         );
    INIT_OPERATION_INFO_( IR_OPERATION_TYPE_LESS    , "<"          );
    INIT_OPERATION_INFO_( IR_OPERATION_TYPE_LESSEQ  , "<="         );
    INIT_OPERATION_INFO_( IR_OPERATION_TYPE_GREAT   , ">"          );
    INIT_OPERATION_INFO_( IR_OPERATION_TYPE_GREATEQ , ">="         );
    INIT_OPERATION_INFO_( IR_OPERATION_TYPE_NONE    , "op_invalid" );

#undef INIT_BLOCK_TYPE_INFO_
// ========================================================================


const IrBlockTypeInfo *GetIrBlockTypeInfo(IrBlockType block_type)
{
    #define TYPE_INFO_CASE_(cmp_block_type_)  case cmp_block_type_: return &struct_info_##cmp_block_type_

    switch (block_type)
    {
        TYPE_INFO_CASE_( IR_BLOCK_TYPE_CALL_FUNCTION );    
        TYPE_INFO_CASE_( IR_BLOCK_TYPE_FUNCTION_BODY );
        TYPE_INFO_CASE_( IR_BLOCK_TYPE_COND_JUMP     );    
        TYPE_INFO_CASE_( IR_BLOCK_TYPE_ASSIGNMENT    );
        TYPE_INFO_CASE_( IR_BLOCK_TYPE_OPERATION     );
        TYPE_INFO_CASE_( IR_BLOCK_TYPE_RETURN        );
        TYPE_INFO_CASE_( IR_BLOCK_TYPE_LOCAL_LABEL   );
        TYPE_INFO_CASE_( IR_BLOCK_TYPE_SYSCALL       );
        TYPE_INFO_CASE_( IR_BLOCK_TYPE_INVALID       );

        default:
            lassert(NULL, "Unknown block type");
            return NULL;
    }

    #undef TYPE_INFO_CASE_
}

const IrOperationTypeInfo *GetIrOperationTypeInfo(IrOperationType operation_type)
{
    #define TYPE_INFO_CASE_(cmp_operation_type_)  case cmp_operation_type_: return &struct_info_##cmp_operation_type_

    switch (operation_type)
    {
        TYPE_INFO_CASE_( IR_OPERATION_TYPE_SUM               );
        TYPE_INFO_CASE_( IR_OPERATION_TYPE_SUB               );
        TYPE_INFO_CASE_( IR_OPERATION_TYPE_MUL               );
        TYPE_INFO_CASE_( IR_OPERATION_TYPE_DIV               );
        TYPE_INFO_CASE_( IR_OPERATION_TYPE_POW               );
        TYPE_INFO_CASE_( IR_OPERATION_TYPE_EQ                );
        TYPE_INFO_CASE_( IR_OPERATION_TYPE_NEQ               );
        TYPE_INFO_CASE_( IR_OPERATION_TYPE_LESS              );
        TYPE_INFO_CASE_( IR_OPERATION_TYPE_LESSEQ            );
        TYPE_INFO_CASE_( IR_OPERATION_TYPE_GREAT             );
        TYPE_INFO_CASE_( IR_OPERATION_TYPE_GREATEQ           );
        TYPE_INFO_CASE_( IR_OPERATION_TYPE_NONE );

        default:
            lassert(NULL, "Unknown operation type");
            return NULL;
    }

    #undef TYPE_INFO_CASE_
}


IrFuncRes IrCtor(IR_struct *ir, size_t start_capacity)
{
    lassert(ir, "");
    lassert(start_capacity > 0, "");

    ir->blocks = (IrBlock *) calloc(start_capacity, sizeof(IrBlock));
    
    if (ir->blocks == NULL)
        log(ERROR, "error in calloc");

    ir->capacity = start_capacity;
    ir->size     = 0;

    ir->proper_names_count = {};
    
    for (size_t i = 0; i < ir->capacity; i++)
    {
        SetPoisonOperationBlock(ir->blocks + i);
    }
    
    IR_VERIFY(ir, IR_FUNC_FAIL);

    return IR_FUNC_OK;
}

IrFuncRes IrDtor(IR_struct *ir)
{
    IR_VERIFY(ir, IR_FUNC_FAIL);

    free(ir->blocks);

    ir->capacity = 0;
    ir->size     = 0;

    return IR_FUNC_OK;
}

IrFuncRes IrRecalloc(IR_struct *ir, size_t new_capacity)
{
    lassert(new_capacity >= ir->capacity, "invalid recalloc size");
    IR_VERIFY(ir, IR_FUNC_FAIL);

    ir->blocks = (IrBlock *) realloc(ir->blocks, new_capacity * sizeof(IrBlock));

    if (ir->blocks == NULL)
        log(ERROR, "error in recalloc");

    size_t prev_capacity = ir->capacity;
    ir->capacity = new_capacity;
    
    for (size_t i = prev_capacity; i < new_capacity; i++)
    {
        SetPoisonOperationBlock(ir->blocks + i);
    }

    IR_VERIFY(ir, IR_FUNC_FAIL);

    return IR_FUNC_OK;
}

IrBlock *IrNewBlock(IR_struct *ir, const IrBlockType block_type, const IrOperationType operation_type, 
                    const IrOperand operand_1, const IrOperand operand_2, const IrOperand ret_operand)
{
    IR_VERIFY(ir, NULL);

    if (ir->size >= ir->capacity)
        IR_ERROR_HANDLE(IrRecalloc(ir, ir->capacity * 2));

    IrBlock *new_block = &ir->blocks[ir->size++];
    lassert(new_block, "");

    new_block->block_type_info = GetIrBlockTypeInfo(block_type);
    new_block->operation_info  = GetIrOperationTypeInfo(operation_type);

    new_block->operand_1   = operand_1;
    new_block->operand_2   = operand_2;
    new_block->ret_operand = ret_operand;

    IR_VERIFY(ir, NULL);

    return new_block;
}

static void SetPoisonOperationBlock(IrBlock *operation_block)
{
    lassert(operation_block, "");

    // operation_block->block_type       = IR_BLOCK_TYPE_INVALID; 
    // operation_block->op_type          = IR_OPERATION_TYPE_NONE;

    operation_block->block_type_info  = GetIrBlockTypeInfo(IR_BLOCK_TYPE_INVALID);      // by default
    operation_block->operation_info   = GetIrOperationTypeInfo(IR_OPERATION_TYPE_NONE);

    operation_block->operand_1.type   = IR_OPERAND_TYPE_NONE;
    operation_block->operand_2.type   = IR_OPERAND_TYPE_NONE;
    operation_block->ret_operand.type = IR_OPERAND_TYPE_NONE;
}

// IrFuncRes BuildIrByTreeNode(IR_struct *ir, Tree *code_tree, Node *cur_node)
// {
//     IR_VERIFY(ir, IR_FUNC_FAIL);
//     lassert(code_tree, "");

//     if (cur_node == NULL)
//         return IR_FUNC_OK;

//     if (cur_node->type == KEY_WORD)
//     {
//         // ...
//     }

//     if (cur_node->type == MATH_OP)
//     {
//         IrBlock *new_operation_block = IrNewBlock(ir);


//     }

//     IR_VERIFY(ir, IR_FUNC_FAIL);
// }

IrBlock *IrBuildAssignBlock(IR_struct *ir, IrOperand operand_1, IrOperand operand_2 /*, IrOperand ret_operand*/)
{
    IR_VERIFY(ir, {});

    IrBlock *assign_block = IrNewBlock(ir, IR_BLOCK_TYPE_ASSIGNMENT, IR_OPERATION_TYPE_NONE, operand_1, operand_2, operand_1);

    IR_VERIFY(ir, {});

    return assign_block;
}


IrOperand GetOperand(IR_struct *ir, Tree *code_tree, Node *cur_node)
{
    IR_VERIFY(ir, {});
    lassert(code_tree, "");
    lassert(cur_node,  "");

    if (cur_node->type == MATH_OP)
    {
        IrOperand operand_1 = GetOperand(ir, code_tree, cur_node->left);
        IrOperand operand_2 = GetOperand(ir, code_tree, cur_node->right);

        
        IrOperationType operation_type = TreeToIrMathOp(cur_node->val.math_op->num);
        
        size_t new_tmp_num = ir->proper_names_count.tmp_count++;
        IrOperand ret_op   = {.type = IR_OPERAND_TYPE_TMP, .val = {.tmp = new_tmp_num}};
        
        IrBlock *new_operation_block = IrNewBlock(ir, IR_BLOCK_TYPE_OPERATION, operation_type, operand_1, operand_2, ret_op);

        return ret_op;
    } 

    else if (cur_node->type == NUM)
    {
        size_t op_tmp_num     = ir->proper_names_count.tmp_count++;
        
        IrOperand op_tmp       = {.type = IR_OPERAND_TYPE_TMP, .val = {.tmp = op_tmp_num}};
        IrOperand assigned_num = {.type = IR_OPERAND_TYPE_NUM, .val = {.num = cur_node->val.num}};

        IrBuildAssignBlock(ir, op_tmp, assigned_num);

        return op_tmp;
    }

    IR_VERIFY(ir, {});
}


static IrOperationType TreeToIrMathOp(MathOperation_enum cur_tree_math_op)
{
    #define SWITCH_MATH_OP(tree_math_op, ir_math_op) case tree_math_op: return ir_math_op

    switch (cur_tree_math_op)
    {
        SWITCH_MATH_OP (TREE_BOOL_EQ         , IR_OPERATION_TYPE_EQ      );
        SWITCH_MATH_OP (TREE_BOOL_NEQ        , IR_OPERATION_TYPE_NEQ     );
        SWITCH_MATH_OP (TREE_BOOL_LOWER      , IR_OPERATION_TYPE_LESS    );
        SWITCH_MATH_OP (TREE_BOOL_GREATER    , IR_OPERATION_TYPE_GREAT   );
        SWITCH_MATH_OP (TREE_BOOL_LOWER_EQ   , IR_OPERATION_TYPE_LESSEQ  );
        SWITCH_MATH_OP (TREE_BOOL_GREATER_EQ , IR_OPERATION_TYPE_GREATEQ );
        SWITCH_MATH_OP (TREE_ADD             , IR_OPERATION_TYPE_SUM     );
        SWITCH_MATH_OP (TREE_SUB             , IR_OPERATION_TYPE_SUB     );
        SWITCH_MATH_OP (TREE_MUL             , IR_OPERATION_TYPE_MUL     );
        SWITCH_MATH_OP (TREE_DIV             , IR_OPERATION_TYPE_DIV     );
        SWITCH_MATH_OP (TREE_DEG             , IR_OPERATION_TYPE_POW     );
        
        case TREE_SQRT:
        case TREE_LN:
        case TREE_LOG2:
        case TREE_SIN:
        case TREE_COS:
        case TREE_TAN:

        default:
            return IR_OPERATION_TYPE_NONE;
    }

    #undef SWITCH_MATH_OP
}


