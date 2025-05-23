#include <stdio.h>
#include <stdlib.h>

#include "ir.h"
#include "logger.h"


FILE *GetInputAST(const int argc, const char *argv[])
{
    lassert(argv);

    FILE *input_ast = NULL;

    if (argc < 2)
        input_ast = fopen(BASE_INPUT_AST_FILE_NAME, "r");

    else
        input_ast = fopen(argv[1], "r");
    
    if (input_ast == NULL)
        log(ERROR, "error opening input_ast file");

    return input_ast;
}


IrFuncRes PrintIrBlock(IrBlock *block, FILE *dest_file)
{
    lassert(block);
    lassert(dest_file);
    
    const char *const block_name     = block->block_type_info->debug_name;
    const char *const operation_name = block->operation_info->debug_name;

    fprintf(dest_file, "%s(", block_name);

    bool is_first = true;
    #define PRINT_COMMA_IF_NEED  if (!is_first) fprintf(dest_file, ", "); else is_first = false
    
    if (block->ret_operand.type != IR_OPERAND_TYPE_NONE)
    {
        PRINT_COMMA_IF_NEED;
        fprintf(dest_file, "%s", GetOperandName(block->ret_operand));
    }

    if (block->block_type_info->type == IR_BLOCK_TYPE_OPERATION)
    {
        PRINT_COMMA_IF_NEED;
        fprintf(dest_file, "%s", operation_name);
    }

    if (block->block_type_info->type == IR_BLOCK_TYPE_FUNCTION_BODY)
    {
        PRINT_COMMA_IF_NEED;
        fprintf(dest_file, "func_%lu_%lu", block->label.func.num, block->label.func.arg_cnt);
    }

    if (block->block_type_info->type == IR_BLOCK_TYPE_LOCAL_LABEL)
    {
        PRINT_COMMA_IF_NEED;
        fprintf(dest_file, "label_%lu", block->label.local);
    }

    if (block->block_type_info->type == IR_BLOCK_TYPE_SYSCALL)
    {
        PRINT_COMMA_IF_NEED;
        fprintf(dest_file, "%s", block->label.sys);
    }

    // if (IS_LABEL_BLOCK_(block))
    // {
    //     PRINT_COMMA_IF_NEED;
    //     fprintf(dest_file, "%s", );
    // }

    if (block->operand_1.type != IR_OPERAND_TYPE_NONE)
    {
        PRINT_COMMA_IF_NEED;
        fprintf(dest_file, "%s", GetOperandName(block->operand_1));
    }

    if (block->operand_2.type != IR_OPERAND_TYPE_NONE)
    {
        PRINT_COMMA_IF_NEED;
        fprintf(dest_file, "%s", GetOperandName(block->operand_2));
    }

    fprintf(dest_file, ")");

    ON_IR_DEBUG(
    if (block->comment[0] != '\0')
        fprintf(dest_file, "\t\t# %s", block->comment);
    )

    fprintf(dest_file, "\n");

    if (block->block_type_info->type == IR_BLOCK_TYPE_RETURN || block->block_type_info->type == IR_BLOCK_TYPE_COND_JUMP)
        fprintf(dest_file, "\n");

    return IR_FUNC_OK;
}


const size_t OPERAND_NAME_MAX_LEN = 50;

const char *GetOperandName(const IrOperand operand)
{
    static char operand_name[OPERAND_NAME_MAX_LEN] = {};
    
    // case cmp_type: snprintf(operand_name, OPERAND_NAME_MAX_LEN, "%s_%lu", name, operand->val)

    IrOperandVal val = operand.val;

    switch (operand.type)
    {
        case IR_OPERAND_TYPE_NUM         : snprintf(operand_name, OPERAND_NAME_MAX_LEN, TREE_ELEM_PRINT_SPECIFIER, val.num);                         break;
        case IR_OPERAND_TYPE_TMP         : snprintf(operand_name, OPERAND_NAME_MAX_LEN, "tmp_%lu",       val.tmp);                                    break;
        case IR_OPERAND_TYPE_VAR         : snprintf(operand_name, OPERAND_NAME_MAX_LEN, "var_%lu",       val.var);                                    break;
        case IR_OPERAND_TYPE_ARG         : snprintf(operand_name, OPERAND_NAME_MAX_LEN, "arg_%lu",       val.arg);                                    break;
        case IR_OPERAND_TYPE_LOCAL_LABEL : snprintf(operand_name, OPERAND_NAME_MAX_LEN, "label_%lu",     val.label.local);                            break;
        case IR_OPERAND_TYPE_FUNC_LABEL  : snprintf(operand_name, OPERAND_NAME_MAX_LEN, "func_%lu_%lu", val.label.func.num, val.label.func.arg_cnt); break;

        case IR_OPERAND_TYPE_NONE        : snprintf(operand_name, OPERAND_NAME_MAX_LEN, "none_operand");                                             break;
    
        default                          : snprintf(operand_name, OPERAND_NAME_MAX_LEN, "invalid_operand_type");                                                                       break;
    }

    return operand_name;
}

