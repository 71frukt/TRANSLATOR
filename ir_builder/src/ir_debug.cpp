#include <stdio.h>
#include <string.h>

#include "ir.h"
#include "ir_debug.h"

static const char *GetIrBlockDumpName (const IrBlock   *const block);

extern FILE *LogFile;

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

    log(LOG, "\tsize     = %ld\n", ir->size);    
    log(LOG, "\tcapacity = %ld\n", ir->capacity);    

    log(LOG, "\tBlocks [%p]:\n", ir->blocks);
    
    IrBlock *blocks = ir->blocks;
    
    for (size_t i = 0; i < ir->size; i++)
    {
        PrintIrBlock(blocks + i, LogFile);   
    }
    BlocksDump_tables(ir);
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


    // <div class="tables-row">
    //     <!-- Таблица 1 -->
    //     <div class="table-with-caption">
    //         <div class="table-caption">[0] = 12345678912345</div>
    //         <table class="vertical-table">
    //             <tr><td></td></tr>
    //             <tr><td></td></tr>
    //             <tr><td></td></tr>
    //             <tr><td></td></tr>
    //         </table>
    //     </div>
        
    //     <!-- Таблица 2 -->
    //     <div class="table-with-caption">
    //         <div class="table-caption">[1] = 12345678912345</div>
    //         <table class="vertical-table">
    //             <tr><td></td></tr>
    //             <tr><td></td></tr>
    //             <tr><td></td></tr>
    //             <tr><td></td></tr>
    //         </table>
    //     </div>
        
    //     <!-- Таблица 3 -->
    //     <div class="table-with-caption">
    //         <div class="table-caption">struct C</div>
    //         <table class="vertical-table">
    //             <tr><td></td></tr>
    //             <tr><td></td></tr>
    //             <tr><td></td></tr>
    //             <tr><td></td></tr>
    //         </table>
    //     </div>
    // </div>
