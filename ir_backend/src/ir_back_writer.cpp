#include <stdio.h>

#include "ir_back_writer.h"
#include "logger.h"
#include "ir.h"

#include "nasm_operations.h"

static IrBackendFuncRes BlockToAsm          (const IrBlock *block           , FILE *dest_file);
static IrBackendFuncRes AssignBlockToAsm    (const IrBlock *assign_block    , FILE *dest_file);
static IrBackendFuncRes OperationBlockToAsm (const IrBlock *operation_block , FILE *dest_file);

static void AddOperationInRaxAsm  (FILE *dest_file, IrOperand operand1, IrOperand operand2);
static void SubOperationInRaxAsm  (FILE *dest_file, IrOperand operand1, IrOperand operand2);
static void MulOperationInRaxAsm  (FILE *dest_file, IrOperand operand1, IrOperand operand2);
static void BoolOperationInRaxAsm (FILE *dest_file, IrOperand operand1, IrOperand operand2, const char *const cond_move_sym);

static void GetOperandValToRegisterAsm(FILE *dest_file, IrOperand operand, const char *const dest_register);

FILE *GetOutputAsmFile(const int argc, const char *argv[])
{
    lassert(argv);

    FILE *input_ast = NULL;

    if (argc < 3)
        input_ast = fopen(BASE_OUTPUT_ASM_FILE_NAME, "w");

    else
        input_ast = fopen(argv[2], "w");
    
    if (input_ast == NULL)
        log(ERROR, "error opening output_asm file");

    return input_ast;
}

IrBackendFuncRes IrToAsmCode(const IR_struct *const ir, FILE *dest_file)
{
    IR_VERIFY(ir, IR_BACK_FUNC_FAIL);
    lassert(dest_file);

    const IrBlock *blocks = ir->blocks;

    for (size_t i = 0; i < ir->size; i++)
    {
        BlockToAsm(blocks + i, dest_file);
    }

    IR_VERIFY(ir, IR_BACK_FUNC_FAIL);
    return IR_BACK_FUNC_OK;
}

IrBackendFuncRes BlockToAsm(const IrBlock *block, FILE *dest_file)
{
    lassert(block);
    lassert(dest_file);
    
    switch (block->block_type_info->type)
    {
    case IR_BLOCK_TYPE_ASSIGNMENT    :  return AssignBlockToAsm    (block, dest_file);
    case IR_BLOCK_TYPE_OPERATION     :  return OperationBlockToAsm (block, dest_file);
    case IR_BLOCK_TYPE_CALL_FUNCTION : 
    case IR_BLOCK_TYPE_FUNCTION_BODY :
    case IR_BLOCK_TYPE_COND_JUMP     :
    case IR_BLOCK_TYPE_NEG_COND_JUMP :
    case IR_BLOCK_TYPE_RETURN        :
    case IR_BLOCK_TYPE_LOCAL_LABEL   :
    case IR_BLOCK_TYPE_SYSCALL       :
    case IR_BLOCK_TYPE_INVALID       :

    default                          : return IR_BACK_FUNC_FAIL;
    }
}

static IrBackendFuncRes AssignBlockToAsm(const IrBlock *assign_block, FILE *dest_file)
{
    lassert(assign_block && assign_block->block_type_info->type == IR_BLOCK_TYPE_ASSIGNMENT);
    lassert(dest_file);

    IrOperand dest_operand   = assign_block->operand_1;
    IrOperand source_operand = assign_block->operand_2;

    lassert(dest_operand.type == IR_OPERAND_TYPE_VAR);
    lassert(source_operand.type == IR_OPERAND_TYPE_NUM || source_operand.type == IR_OPERAND_TYPE_TMP);

    if (source_operand.type == IR_OPERAND_TYPE_TMP)
    {
        fprintf(dest_file, POP_(RAX_));
        fprintf(dest_file, MOV_(_VAR_, RAX_), dest_operand.val.var);
    }

    else    // if type == num
        fprintf(dest_file, MOV_(_VAR_, _NUM_), dest_operand.val.var, source_operand.val.num);

    return IR_BACK_FUNC_OK;
}

static IrBackendFuncRes OperationBlockToAsm(const IrBlock *operation_block, FILE *dest_file)
{
    lassert(operation_block);
    lassert(dest_file);

    IrOperand operand1 = operation_block->operand_1;
    IrOperand operand2 = operation_block->operand_2;

    switch (operation_block->operation_info->type)
    {
    case IR_OPERATION_TYPE_EQ      :  BoolOperationInRaxAsm (dest_file, operand1, operand2, CMOVE_SYM );    break;
    case IR_OPERATION_TYPE_NEQ     :  BoolOperationInRaxAsm (dest_file, operand1, operand2, CMOVNE_SYM);    break;
    case IR_OPERATION_TYPE_LESS    :  BoolOperationInRaxAsm (dest_file, operand1, operand2, CMOVL_SYM );    break;
    case IR_OPERATION_TYPE_LESSEQ  :  BoolOperationInRaxAsm (dest_file, operand1, operand2, CMOVLE_SYM);    break;
    case IR_OPERATION_TYPE_GREAT   :  BoolOperationInRaxAsm (dest_file, operand1, operand2, CMOVG_SYM );    break;
    case IR_OPERATION_TYPE_GREATEQ :  BoolOperationInRaxAsm (dest_file, operand1, operand2, CMOVGE_SYM);    break;

    case IR_OPERATION_TYPE_ADD     :  AddOperationInRaxAsm  (dest_file, operand1, operand2);                break;
    case IR_OPERATION_TYPE_SUB     :  SubOperationInRaxAsm  (dest_file, operand1, operand2);                break;
    case IR_OPERATION_TYPE_MUL     :  MulOperationInRaxAsm  (dest_file, operand1, operand2);                break;


    case IR_OPERATION_TYPE_DIV     :
    case IR_OPERATION_TYPE_POW     :
    case IR_OPERATION_TYPE_NONE    :
    default                        :    return IR_BACK_FUNC_FAIL;
    }

    fprintf(dest_file, PUSH_(RAX_));
    return IR_BACK_FUNC_OK;
}


// static IrBackendFuncRes BoolBlockToAsm(const IrBlock *bool_block, FILE *dest_file)
// {
//     lassert(bool_block);
//     lassert(dest_file);

//     IrOperand operand1 = bool_block->operand_1;
//     IrOperand operand2 = bool_block->operand_2;

//     fprintf(dest_file, CMP_SYM);

//     if (operand1.type == IR_OPERAND_TYPE_NUM)
//         fprintf(dest_file, _NUM_, operand1.val.num);

//     const char *cmove_str = NULL;

//     switch (bool_block->block_type_info->type)
//     {
//     case IR_OPERATION_TYPE_EQ      :  BoolOperationInRaxAsm (dest_file, operand1, operand2, CMOVE_SYM );    break;
//     case IR_OPERATION_TYPE_NEQ     :  BoolOperationInRaxAsm (dest_file, operand1, operand2, CMOVNE_SYM);    break;
//     case IR_OPERATION_TYPE_LESS    :  BoolOperationInRaxAsm (dest_file, operand1, operand2, CMOVL_SYM );    break;
//     case IR_OPERATION_TYPE_LESSEQ  :  BoolOperationInRaxAsm (dest_file, operand1, operand2, CMOVLE_SYM);    break;
//     case IR_OPERATION_TYPE_GREAT   :  BoolOperationInRaxAsm (dest_file, operand1, operand2, CMOVG_SYM );    break;
//     case IR_OPERATION_TYPE_GREATEQ :  BoolOperationInRaxAsm (dest_file, operand1, operand2, CMOVGE_SYM);    break;

//     case IR_OPERATION_TYPE_MUL     :  MulOperationInRaxAsm  (dest_file, operand1, operand2);                break;


//     default                        :    return IR_BACK_FUNC_FAIL;
//     }

//     return IR_BACK_FUNC_OK;
// }


static void AddOperationInRaxAsm(FILE *dest_file, IrOperand operand1, IrOperand operand2)
{
    lassert(dest_file);

    GetOperandValToRegisterAsm(dest_file, operand2, RBX_);
    GetOperandValToRegisterAsm(dest_file, operand1, RAX_);

    fprintf(dest_file, ADD_(RAX_, RBX_));
}

static void SubOperationInRaxAsm(FILE *dest_file, IrOperand operand1, IrOperand operand2)
{
    lassert(dest_file);

    GetOperandValToRegisterAsm(dest_file, operand2, RBX_);
    GetOperandValToRegisterAsm(dest_file, operand1, RAX_);

    fprintf(dest_file, SUB_(RAX_, RBX_));
}

static void MulOperationInRaxAsm(FILE *dest_file, IrOperand operand1, IrOperand operand2)
{
    lassert(dest_file);

    GetOperandValToRegisterAsm(dest_file, operand2, RBX_);
    GetOperandValToRegisterAsm(dest_file, operand1, RAX_);

    fprintf(dest_file, MUL_(RBX_));
}


static void BoolOperationInRaxAsm(FILE *dest_file, IrOperand operand1, IrOperand operand2, const char *const cond_move_sym)
{
    lassert(dest_file);
    lassert(cond_move_sym);

    GetOperandValToRegisterAsm(dest_file, operand2, RBX_);
    GetOperandValToRegisterAsm(dest_file, operand1, RAX_);
    fprintf(dest_file, CMP_(RAX_, RBX_));
    fprintf(dest_file, MOV_(RAX_, "0"));
    fprintf(dest_file, "\t%s" RAX_ ", 1\n", cond_move_sym);
}


static void GetOperandValToRegisterAsm(FILE *dest_file, IrOperand operand, const char *const dest_register)
{
    lassert(operand.type == IR_OPERAND_TYPE_NUM || operand.type == IR_OPERAND_TYPE_VAR || operand.type == IR_OPERAND_TYPE_TMP);

    if (operand.type == IR_OPERAND_TYPE_TMP)
        fprintf(dest_file, POP_("%s"), dest_register);

    else if (operand.type == IR_OPERAND_TYPE_VAR)
        fprintf(dest_file, MOV_("%s", _VAR_), dest_register, operand.val.var);

    else // if type == NUM
        fprintf(dest_file, MOV_("%s", _NUM_), dest_register, operand.val.num);
}