#include <stdio.h>

#include "ir_back_writer.h"
#include "logger.h"
#include "ir.h"

#include "nasm_operations.h"

static IrBackendFuncRes BlockToAsm          (const IrBlock *block            , FILE *dest_file);
static IrBackendFuncRes AssignBlockToAsm    (const IrBlock *assign_block     , FILE *dest_file);
static IrBackendFuncRes OperationBlockToAsm (const IrBlock *operation_block  , FILE *dest_file);
static IrBackendFuncRes LocalLabelToAsm     (const IrBlock *local_label_block, FILE *dest_file);
static IrBackendFuncRes CondJumpToAsm       (const IrBlock *local_label_block, FILE *dest_file, const char *const cnd_jmp_sym);
static IrBackendFuncRes CallFuncToAsm       (const IrBlock *call_func_block  , FILE *dest_file);
static IrBackendFuncRes FuncBodyToAsm       (const IrBlock *func_body_block  , FILE *dest_file);
static IrBackendFuncRes FuncReturnToAsm     (const IrBlock *func_return_block, FILE *dest_file);

static IrBackendFuncRes FuncFirstArgToAsm   (IrOperand arg_source_operand,                    FILE *dest_file);
static IrBackendFuncRes AssignVarToAsm      (IrOperand var_operand, IrOperand source_operand, FILE *dest_file);
static IrBackendFuncRes AssignArgToAsm      (IrOperand var_operand, IrOperand source_operand, FILE *dest_file);


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
    case IR_BLOCK_TYPE_LOCAL_LABEL   :  return LocalLabelToAsm     (block, dest_file);
    case IR_BLOCK_TYPE_COND_JUMP     :  return CondJumpToAsm       (block, dest_file, JNE_SYM);
    case IR_BLOCK_TYPE_NEG_COND_JUMP :  return CondJumpToAsm       (block, dest_file, JE_SYM);
    case IR_BLOCK_TYPECALL_FUNCTION :  return CallFuncToAsm       (block, dest_file);
    case IR_BLOCK_TYPE_FUNCTION_BODY :  return FuncBodyToAsm       (block, dest_file);
    case IR_BLOCK_TYPE_RETURN        :  return FuncReturnToAsm     (block, dest_file);
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

    if (dest_operand.type == IR_OPERAND_TYPE_VAR)
        return AssignVarToAsm(dest_operand, source_operand, dest_file);

    else    // if type == arg
        return AssignArgToAsm(dest_operand, source_operand, dest_file);
}

static IrBackendFuncRes AssignVarToAsm(IrOperand var_operand, IrOperand source_operand, FILE *dest_file)
{
    lassert(var_operand.type == IR_OPERAND_TYPE_VAR);
    lassert(source_operand.type == IR_OPERAND_TYPE_NUM || source_operand.type == IR_OPERAND_TYPE_TMP);
    lassert(dest_file);

    if (source_operand.type == IR_OPERAND_TYPE_TMP)
    {
        fprintf(dest_file, POP_(RAX_));
        fprintf(dest_file, MOV_(_VAR_, RAX_), var_operand.val.var);
    }

    else    // if type == num
        fprintf(dest_file, MOV_(_VAR_, _NUM_), var_operand.val.var, source_operand.val.num);

    return IR_BACK_FUNC_OK;
}

static IrBackendFuncRes AssignArgToAsm(IrOperand arg_operand, IrOperand source_operand, FILE *dest_file)
{
    lassert(arg_operand.type == IR_OPERAND_TYPE_ARG);
    lassert(source_operand.type == IR_OPERAND_TYPE_NUM || source_operand.type == IR_OPERAND_TYPE_TMP);
    lassert(dest_file);

    if (arg_operand.val.arg == 0)   // the first arg of func call -> free space for future ret addr
        return FuncFirstArgToAsm(source_operand, dest_file);

    if (source_operand.type != IR_OPERAND_TYPE_TMP)
        fprintf(dest_file, PUSH_(QWORD_SYM _VAR_), source_operand.val.var);
    
    // else argument is inited by tmp that is already in stack

    return IR_BACK_FUNC_OK;
}


static IrBackendFuncRes FuncFirstArgToAsm(IrOperand arg_source_operand, FILE *dest_file)
{
    if (arg_source_operand.type == IR_OPERAND_TYPE_TMP)
    {
        GetOperandValToRegisterAsm(dest_file, arg_source_operand, RAX_);
        fprintf(dest_file, SUB_(RSP_, "8"));
        fprintf(dest_file, PUSH_(RBP_));
        fprintf(dest_file, PUSH_(RAX_));
    }

    else
    {
        fprintf(dest_file, SUB_(RSP_, "8"));
        fprintf(dest_file, PUSH_(RBP_));
        fprintf(dest_file, PUSH_(QWORD_SYM _VAR_), arg_source_operand.val.var);
    }

    fprintf(dest_file, MOV_(RBP_, RSP_));

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


static IrBackendFuncRes LocalLabelToAsm(const IrBlock *local_label_block, FILE *dest_file)
{
    lassert(local_label_block && local_label_block->block_type_info->type == IR_BLOCK_TYPE_LOCAL_LABEL);
    lassert(dest_file);

    fprintf(dest_file, _DEF_LOCAL_LABEL_, local_label_block->label.local);

    return IR_BACK_FUNC_OK;
}


static IrBackendFuncRes CondJumpToAsm(const IrBlock *local_label_block, FILE *dest_file, const char *const cnd_jmp_sym)
{
    lassert(local_label_block);
    lassert(local_label_block->block_type_info->type == IR_BLOCK_TYPE_COND_JUMP || local_label_block->block_type_info->type == IR_BLOCK_TYPE_NEG_COND_JUMP);
    lassert(cnd_jmp_sym);

    IrOperand label_operand     = local_label_block->operand_1;
    IrOperand condition_operand = local_label_block->operand_2;

    lassert(label_operand.type     == IR_OPERAND_TYPE_LOCAL_LABEL);
    lassert(condition_operand.type == IR_OPERAND_TYPE_TMP);

    GetOperandValToRegisterAsm(dest_file, condition_operand, RAX_);

    fprintf(dest_file, TEST_(RAX_, RAX_));

    fprintf(dest_file, "\t%s" _LOCAL_LABEL_ "\n", cnd_jmp_sym, label_operand.val.label.local);

    return IR_BACK_FUNC_OK;
}


static IrBackendFuncRes CallFuncToAsm(const IrBlock *call_func_block, FILE *dest_file)
{
    lassert(call_func_block && call_func_block->block_type_info->type == IR_BLOCK_TYPECALL_FUNCTION);
    lassert(dest_file);

    // stack frame debug in FuncFirstArgToAsm()

    IrOperand func_label = call_func_block->operand_1;
    lassert(func_label.type == IR_OPERAND_TYPE_FUNC_LABEL);

    fprintf(dest_file, CALL_, func_label.val.label.func.num, func_label.val.label.func.arg_cnt);

    // ret_addr to space before args  in FuncBodyToAsm()

    return IR_BACK_FUNC_OK;
}


static IrBackendFuncRes FuncBodyToAsm(const IrBlock *func_body_block, FILE *dest_file)
{
    lassert(func_body_block && func_body_block->block_type_info->type == IR_BLOCK_TYPE_FUNCTION_BODY);
    lassert(dest_file);

    size_t func_num = func_body_block->label.func.num;
    size_t arg_cnt  = func_body_block->label.func.arg_cnt;

    fprintf(dest_file, _FUNC_INIT_, func_num, arg_cnt);

    fprintf(dest_file, POP_(RAX_));     // ret addr -> rax
    
    size_t shift = arg_cnt + 1;   // arg_cnt + saved rbp
    
    fprintf(dest_file, MOV_("[" RSP_ "+ %lu]", RAX_), shift);  // rax = ret_addr -> before saved rbp

    return IR_BACK_FUNC_OK;
}


static IrBackendFuncRes FuncReturnToAsm(const IrBlock *func_return_block, FILE *dest_file)
{
    lassert(func_return_block && func_return_block->block_type_info->type == IR_BLOCK_TYPE_RETURN);
    lassert(dest_file);

    fprintf(dest_file, MOV_(RSP_, RBP_));
    fprintf(dest_file, ADD_(RSP_, "8"));
    fprintf(dest_file, POP_(RBP_));
    fprintf(dest_file, RET_);

    return IR_BACK_FUNC_OK;
}


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