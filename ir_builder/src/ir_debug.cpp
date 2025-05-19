#include <stdio.h>
#include <string.h>

#include "ir.h"
#include "ir_debug.h"

static const char *GetIrBlockDumpName (const IrBlock   *const block);

int IrVerify(const IR_struct *ir)
{
    int res_err = IR_OK;

    if (ir == NULL)
        return IR_PTR_ERR;
    
    if (ir->blocks == NULL)
        return IR_DATA_PTR_ERR;

    if (ir->capacity < ir->size)
        res_err |= IR_OVERFLOW;

    return res_err;
}


char *GetIrErrors(int error)
{
    static char error_str[ERROR_STR_MAX_SIZE];
    size_t error_str_cursor = 0;

    #define PRINT_ERROR_PART(full_err, cmp_err)                                         \
        if (full_err & cmp_err)                                                         \
        {                                                                               \
            strncpy(error_str, #cmp_err, ERROR_STR_MAX_SIZE - error_str_cursor - 1);    \
            error_str_cursor += strlen(#cmp_err);                                       \
                                                                                        \
            if (error_str_cursor == ERROR_STR_MAX_SIZE)                                 \
            {                                                                           \
                log(WARNING, "error_str overflow");                                     \
            }                                                                           \
        }

    PRINT_ERROR_PART(error, IR_PTR_ERR);
    PRINT_ERROR_PART(error, IR_DATA_PTR_ERR);
    PRINT_ERROR_PART(error, IR_OVERFLOW);

    return error_str;

    #undef PRINT_ERROR_PART
}

void IrDump(const IR_struct *ir)
{
    log(LOG, "IR [%p]\n", ir);
    log(LOG, "{\n");

    DefaultTabNum++;

    log(LOG, "size     = %ld\n", ir->size);    
    log(LOG, "capacity = %ld\n", ir->capacity);    

    log(LOG, "IR code [%p]:\n\n", ir->blocks);

    DefaultTabNum--;
    log(LOG, "---------------------- CODE --------------------------------------\n");
    DefaultTabNum++;
    
    IrBlock *blocks = ir->blocks;
    
    for (size_t i = 0; i < ir->size; i++)
    {
        if (IS_LABEL_BLOCK_(blocks + i))
        {
            log(LOG, "\n");
            DefaultTabNum--;
            log(LOG, "");
            DefaultTabNum++;
        }
        
        else
           log(LOG, "");

        PrintIrBlock(blocks + i, LogFile);   
    }

    DefaultTabNum--;
    log(LOG, "------------------------------------------------------------------\n\n");

    log(LOG, "\n");
    log(LOG, "}\n\n\n");
}


void BlocksDump_tables(const IR_struct *ir)
{
    log(LOG, "<div class=\"tables-row\">\n");

    IrBlock *blocks = ir->blocks;

    for (size_t i = 0; i < ir->size; i++)
    {
        log(LOG, "\t<div class=\"table-with-caption\">\n");

        log(LOG, "\t\t<div class=\"table-caption\">blocks[%lu]</div>\n", i);
        log(LOG, "\t\t<table class=\"table\">\n");

        log(LOG, "\t\t<tr><td>%s</td></tr>\n", GetIrBlockDumpName(blocks + i));

        if (blocks != NULL)
        {
            log(LOG, "\t\t<tr><td>%s</td></tr>\n", GetOperandName(blocks[i].operand_1));
            log(LOG, "\t\t<tr><td>%s</td></tr>\n", GetOperandName(blocks[i].operand_2));
            log(LOG, "\t\t<tr><td>%s</td></tr>\n", GetOperandName(blocks[i].ret_operand));
        }

        else
        {
            log(LOG, "\t\t<tr><td></td></tr>\n");
            log(LOG, "\t\t<tr><td></td></tr>\n");
            log(LOG, "\t\t<tr><td></td></tr>\n");
        }

        log(LOG, "\t\t</table>\n");
        log(LOG, "\t</div>\n");
    }

    log(LOG, " </div>\n");
}


static const char *GetIrBlockDumpName(const IrBlock *const block)
{
    if (block == NULL)
        return "nullptr";

    if (block->block_type_info->type == IR_BLOCK_TYPE_OPERATION)
        return block->operation_info->debug_name;

    else
        return block->block_type_info->debug_name;
}

void LoadOperandRealName(IrOperand *named_operand, const Node *named_node)
{
    lassert(named_node);
    lassert(named_node->type == NODE_FUNC || named_node->type == NODE_VAR);
    snprintf(named_operand->comment_name, OPERAND_NAME_LEN, "%s", named_node->val.prop_name->name);
}

void MakeBlockComment(IrBlock *block)
{
    char *comment = block->comment;

    switch (block->block_type_info->type)
    {
    case IR_BLOCK_TYPE_ASSIGNMENT:
        if (block->operand_1.type == IR_OPERAND_TYPE_VAR)
            snprintf(comment, COMMENT_LEN, "int %s", block->operand_1.comment_name);
        break;

    case IR_BLOCK_TYPECALL_FUNCTION:
        snprintf(comment, COMMENT_LEN, "call %s", block->operand_1.comment_name);
        break;


    case IR_BLOCK_TYPE_FUNCTION_BODY:
    case IR_BLOCK_TYPE_COND_JUMP:
    case IR_BLOCK_TYPE_NEG_COND_JUMP:
    case IR_BLOCK_TYPE_OPERATION:
    case IR_BLOCK_TYPE_RETURN:
    case IR_BLOCK_TYPE_LOCAL_LABEL:
    case IR_BLOCK_TYPE_SYSCALL:
    case IR_BLOCK_TYPE_INVALID:
    
    default:
        break;
    }
}

