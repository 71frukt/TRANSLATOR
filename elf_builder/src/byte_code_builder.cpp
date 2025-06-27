#include <stdio.h>

#include "elf_info.h"
#include "ir.h"
#include "byte_code_builder.h"
#include "instructions_to_bytes.h"


static ElfFuncRes AssignBlockToBytes    (const IrBlock *assign_block, TextSection *text);
static ElfFuncRes OperationBlockToBytes (const IrBlock *operation_block, TextSection *text);

static ElfFuncRes AddOperationInRaxBytes(TextSection *text, IrOperand operand1, IrOperand operand2);


static void GetOperandValToRegisterBytes(TextSection *text, IrOperand operand, RegCode dest_reg_code);


ElfFuncRes GetByteCodeFromIR(IR_struct *ir, TextSection *text_sect)
{
    // FIXME: заменить эту хуйню на построение реального кода

    // text_sect->size = 12;
    // text_sect->code = (char *) calloc(12, sizeof(char));

    char my_code[12] = 
    {
        0xb8, 0x3c, 0x00, 0x00, 0x00,  // mov eax, 60 (syscall exit)
        0xbf, 0x7b, 0x00, 0x00, 0x00,  // mov edi, 666 (аргумент)
        0x0f, 0x05                     // syscall
    };


    for (size_t i = 0; i < 12; i++)
    {
        text_sect->code[i] = my_code[i];
    }

    return ELF_FUNC_OK;
}




ElfFuncRes IrBlockToBytes(const IrBlock *block, TextSection *text)
{
    lassert(block);
    lassert(text);
    
    switch (block->block_type_info->type)
    {
    case IR_BLOCK_TYPE_ASSIGNMENT    :  return AssignBlockToBytes    (block, text);
    case IR_BLOCK_TYPE_OPERATION     :  return OperationBlockToBytes (block, text);
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

static ElfFuncRes AssignBlockToBytes(const IrBlock *assign_block, TextSection *text)    // TODO: дописать
{
    lassert(assign_block && assign_block->block_type_info->type == IR_BLOCK_TYPE_ASSIGNMENT);
    lassert(text);

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

    case IR_OPERATION_TYPE_ADD     :  AddOperationInRaxBytes(text, operand1, operand2); break;
    case IR_OPERATION_TYPE_SUB     :  break;
    case IR_OPERATION_TYPE_MUL     :  break;
    case IR_OPERATION_TYPE_DIV     :  break;
    case IR_OPERATION_TYPE_SQRT    :  break;

    case IR_OPERATION_TYPE_POW     :
    case IR_OPERATION_TYPE_NONE    :
    default                        :    return ELF_FUNC_FAIL;
    }

    // fprintf(dest_file, PUSH_(RAX_));
    return ELF_FUNC_OK;
}


static ElfFuncRes AddOperationInRaxBytes(TextSection *text, IrOperand operand1, IrOperand operand2)
{
    lassert(text);

    GetOperandValToRegisterBytes(text, operand2, RBX_CODE);
    GetOperandValToRegisterBytes(text, operand1, RAX_CODE);

    return ELF_FUNC_OK;
}


static void GetOperandValToRegisterBytes(TextSection *text, IrOperand operand, RegCode dest_reg_code)
{
    lassert(text);
    lassert(operand.type == IR_OPERAND_TYPE_NUM || operand.type == IR_OPERAND_TYPE_VAR || operand.type == IR_OPERAND_TYPE_TMP);

    if (operand.type == IR_OPERAND_TYPE_TMP)
        PopReg(text, dest_reg_code);

    else if (operand.type == IR_OPERAND_TYPE_VAR)
        MovRegMem(text, dest_reg_code, RBP_CODE, -8 * (int) operand.val.var);

    else // if type == NUM
        MovRegImm(text, dest_reg_code, operand.val.num);
}


// static void InstrBytesInTextSection(const char *instr_bytes, const size_t instr_size, TextSection *text)
// {
//     lassert(instr_bytes);
//     lassert(text);

//     for (size_t i = 0; i < instr_size; i++)
//         text->code[text->size + i] + instr_bytes[i];

//     text->size += instr_size;
// }


