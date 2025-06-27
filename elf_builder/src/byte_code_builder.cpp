#include <stdio.h>

#include "elf_info.h"
#include "ir.h"
#include "byte_code_builder.h"
#include "instructions_to_bytes.h"


static ElfFuncRes AssignBlockToBytes    (const IrBlock *assign_block, TextSection *text);
static ElfFuncRes OperationBlockToBytes (const IrBlock *operation_block, TextSection *text);

static ElfFuncRes AddOperationInRaxBytes(TextSection *text, IrOperand operand1, IrOperand operand2);

static ElfFuncRes AssignVarToBytes(IrOperand var_operand, IrOperand source_operand, TextSection *text);
static ElfFuncRes AssignArgToBytes(IrOperand arg_operand, IrOperand source_operand, TextSection *text);

static void GetOperandValToRegisterBytes(TextSection *text, IrOperand operand, RegCode dest_reg_code);


ElfFuncRes GetByteCodeFromIR(IR_struct *ir, TextSection *text_sect)
{
    IR_VERIFY(ir, ELF_FUNC_FAIL);
    lassert(text_sect);

    const IrBlock *blocks = ir->blocks;

    memcpy(text_sect->code + text_sect->size, START_PREAMBLE, START_PREAMBLE_LEN);
    text_sect->size += START_PREAMBLE_LEN;

    for (size_t i = 0; i < ir->size; i++)
        ELF_HANDLE(IrBlockToBytes(blocks + i, text_sect));

    memcpy(text_sect->code + text_sect->size, EXIT_PREAMBLE, EXIT_PREAMBLE_LEN);
    text_sect->size += EXIT_PREAMBLE_LEN;

    log(INFO, "Res code:\n");
    for (size_t i = 0; i < text_sect->size; i++)
        fprintf(stderr, "%02x ", (unsigned char) text_sect->code[i]);

    IR_VERIFY(ir, ELF_FUNC_FAIL);
    return ELF_FUNC_OK;
}




ElfFuncRes IrBlockToBytes(const IrBlock *block, TextSection *text)
{
    lassert(block);
    lassert(text);
    
    switch (block->block_type_info->type)
    {
    case IR_BLOCK_TYPE_ASSIGNMENT    :  return AssignBlockToBytes    (block, text); 
    case IR_BLOCK_TYPE_OPERATION     :  return OperationBlockToBytes (block, text); // TODO дописать
    case IR_BLOCK_TYPE_LOCAL_LABEL   :
    case IR_BLOCK_TYPE_COND_JUMP     :
    case IR_BLOCK_TYPE_NEG_COND_JUMP :
    case IR_BLOCK_TYPE_CALL_FUNCTION :
    case IR_BLOCK_TYPE_FUNCTION_BODY :
    case IR_BLOCK_TYPE_RETURN        :
    case IR_BLOCK_TYPE_SYSCALL       :
    case IR_BLOCK_TYPE_INVALID       :

    default                          : return ELF_FUNC_FAIL;
    }
}

static ElfFuncRes AssignBlockToBytes(const IrBlock *assign_block, TextSection *text)
{
    lassert(assign_block && assign_block->block_type_info->type == IR_BLOCK_TYPE_ASSIGNMENT);
    lassert(text);

    IrOperand dest_operand   = assign_block->operand_1;
    IrOperand source_operand = assign_block->operand_2;

    if (dest_operand.type == IR_OPERAND_TYPE_VAR)
        return AssignVarToBytes(dest_operand, source_operand, text);

    else    // if type == arg
        return AssignArgToBytes(dest_operand, source_operand, text);

    return ELF_FUNC_OK;
}

static ElfFuncRes AssignVarToBytes(IrOperand var_operand, IrOperand source_operand, TextSection *text)
{
    lassert(var_operand.type == IR_OPERAND_TYPE_VAR);
    lassert(source_operand.type == IR_OPERAND_TYPE_NUM || source_operand.type == IR_OPERAND_TYPE_TMP);
    lassert(text);

    log(INFO, "var_num = %lu, num = %d", var_operand.val.var, source_operand.val.num);

    if (source_operand.type == IR_OPERAND_TYPE_TMP)
    {
        PopReg(text, RAX_CODE);
        MovVarReg(text, var_operand.val.var, RAX_CODE);

        // fprintf(dest_file, POP_(RAX_));
        // fprintf(dest_file, MOV_(_VAR_, RAX_), var_operand.val.var);
    }

    else    // if type == num
        MovVarImm(text, var_operand.val.var, source_operand.val.num);
        // fprintf(dest_file, MOV_(_VAR_, _NUM_), var_operand.val.var, source_operand.val.num);
        
    SubRegImm(text, RSP_CODE, 8);
    // fprintf(dest_file, SUB_(RSP_, "8" COMM_("place for var")));

    return ELF_FUNC_OK;
}


static ElfFuncRes AssignArgToBytes(IrOperand arg_operand, IrOperand source_operand, TextSection *text)
{
    lassert(arg_operand.type == IR_OPERAND_TYPE_ARG);
    lassert(source_operand.type == IR_OPERAND_TYPE_NUM || source_operand.type == IR_OPERAND_TYPE_TMP || source_operand.type == IR_OPERAND_TYPE_VAR);
    lassert(text);

    // if (arg_operand.val.arg == 0)   // the first arg of func call -> free space for future ret addr  // FIXME: добавить
    //     return FuncFirstArgToAsm(source_operand, dest_file);

    if (source_operand.type == IR_OPERAND_TYPE_NUM)
        PushImm(text, source_operand.val.num);

    else if (source_operand.type == IR_OPERAND_TYPE_VAR)
        PushVar(text, source_operand.val.var);

    // else argument is inited by tmp that is already in stack

    return ELF_FUNC_OK;
}


static ElfFuncRes OperationBlockToBytes(const IrBlock *operation_block, TextSection *text)
{
    lassert(operation_block && operation_block->block_type_info->type == IR_BLOCK_TYPE_OPERATION);
    lassert(text);

    IrOperand operand1 = operation_block->operand_1;
    IrOperand operand2 = operation_block->operand_2;

    switch (operation_block->operation_info->type)
    {
    case IR_OPERATION_TYPE_EQ      :  break; // BoolOperationInRaxBytes (dest_file, operand1, operand2, CMOVE_SYM );    break;
    case IR_OPERATION_TYPE_NEQ     :  break;
    case IR_OPERATION_TYPE_LESS    :  break;
    case IR_OPERATION_TYPE_LESSEQ  :  break;
    case IR_OPERATION_TYPE_GREAT   :  break;
    case IR_OPERATION_TYPE_GREATEQ :  break;

    case IR_OPERATION_TYPE_ADD     :  AddOperationInRaxBytes(text, operand1, operand2); break;  // TODO дописать
    case IR_OPERATION_TYPE_SUB     :  break;
    case IR_OPERATION_TYPE_MUL     :  break;
    case IR_OPERATION_TYPE_DIV     :  break;
    case IR_OPERATION_TYPE_SQRT    :  break;

    case IR_OPERATION_TYPE_POW     :
    case IR_OPERATION_TYPE_NONE    :
    default                        :    return ELF_FUNC_FAIL;
    }

    PushReg(text, RAX_CODE);
    // fprintf(dest_file, PUSH_(RAX_));
    return ELF_FUNC_OK;
}


static ElfFuncRes AddOperationInRaxBytes(TextSection *text, IrOperand operand1, IrOperand operand2)
{
    lassert(text);

    GetOperandValToRegisterBytes(text, operand2, RBX_CODE);
    GetOperandValToRegisterBytes(text, operand1, RAX_CODE);

    AddRegReg(text, RAX_CODE, RBX_CODE);

    return ELF_FUNC_OK;
}


static void GetOperandValToRegisterBytes(TextSection *text, IrOperand operand, RegCode dest_reg_code)
{
    lassert(text);
    lassert(operand.type == IR_OPERAND_TYPE_NUM || operand.type == IR_OPERAND_TYPE_VAR || operand.type == IR_OPERAND_TYPE_TMP);

    if (operand.type == IR_OPERAND_TYPE_TMP)
        PopReg(text, dest_reg_code);

    else if (operand.type == IR_OPERAND_TYPE_VAR)
        MovRegVar(text, dest_reg_code, operand.val.var);

    else // if type == NUM
        MovRegImm(text, dest_reg_code, operand.val.num);
}