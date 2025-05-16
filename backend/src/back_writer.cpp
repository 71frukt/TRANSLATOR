#include <stdio.h>
#include <assert.h>

#include "tree_lib.h"
#include "back_writer.h"


FILE *GetOutputAsmFile(const int argc, const char *argv[])
{
    FILE *output_file = NULL;

    if (argc < 2)
        output_file = fopen(BASE_OUTPUT_ASM_FILE_NAME, "w");
    
    else
        output_file = fopen(argv[2], "w");

    return output_file;   
}

void PrintStartRegisterValues(FILE *dest_file)
{
    fprintf(dest_file,  "PUSH 0                     \n"
                        "POP AX                   \n\n"
        
                        "PUSH 0                     \n"
                        "POP BX                   \n\n"
        
                        "PUSH AX                    \n"
                        "PUSH 0                   \n\n"
        
                        "CALL " MAIN_FUNC_NAME ":   \n"
                        "TREE_SPU_OUT                    \n"
                        "HLT                        \n"
    );
}

void PrintMathOpAsm(Node *math_op, FILE *dest_file)
{
    assert(math_op);
    assert(math_op->type == MATH_OP);
    assert(dest_file);

    PrintAsmCodeByNode(math_op->left,  dest_file);

    if (math_op->val.math_op->type == BINARY)
        PrintAsmCodeByNode(math_op->right, dest_file);

    fprintf(dest_file, "%s ", MathOpAsmInfos[math_op->val.math_op->num].asm_symbol);

    if (!TREE_NODE_IS_BOOL(math_op))
        fprintf(dest_file, "\n");
}

void PrintVarTAsm(Node *var_t_node, FILE *dest_file)
{
    assert(var_t_node);
    assert(var_t_node->type == KEY_WORD && var_t_node->val.key_word->name == TREE_VAR_T_INDICATOR);

    fprintf(dest_file, "%s [AX + %lu]\n", AsmOperations[PUSH_ASM].sym, var_t_node->left->val.prop_name->number);
}

void PrintChildrenAsm(Node *new_expr_node, FILE *dest_file)
{
    assert(new_expr_node);
    assert(dest_file);

    fprintf(dest_file, "\n");

    if (new_expr_node->left != NULL)
        PrintAsmCodeByNode(new_expr_node->left, dest_file);

    if (new_expr_node->right != NULL)
        PrintAsmCodeByNode(new_expr_node->right, dest_file);
}

void PopToEmptyRam(FILE *asm_file)                          // pop [BX++]   // BX - размер, AX - начало фрейма
{
    fprintf(asm_file, "\n%s [BX] \n", AsmOperations[POP_ASM].sym);

    fprintf(asm_file, "%s BX   \n"                     // BX ++
                      "%s 1    \n"
                      "%s      \n"
                      "%s BX   \n", 
        AsmOperations[PUSH_ASM].sym, AsmOperations[PUSH_ASM].sym, AsmOperations[ADD_ASM].sym, AsmOperations[POP_ASM].sym);
}

void PrintInitAsm(Node *init_node, FILE *dest_file)
{
    assert(init_node);
    assert(dest_file);
    assert(init_node->type == KEY_WORD && (init_node->val.key_word->name == TREE_INT_INIT || init_node->val.key_word->name == TREE_DOUBLE_INIT));

    Node *prop_name_node = init_node->left->left;
    assert(prop_name_node->type == VAR || prop_name_node->type == FUNC);

    if(init_node->left->type == KEY_WORD && init_node->left->val.key_word->name == TREE_VAR_T_INDICATOR)     // переменная
    {
        fprintf(dest_file, "\t%c инициализация переменной '%s'   \n", COMMENT_SYMBOL, prop_name_node->val.prop_name->name);    

        if (init_node->right != NULL)                                                                   // нет инициализации, например, при объявлении функции  func(int a) в теле будет только pop[BX++]
            PrintAsmCodeByNode(init_node->right, dest_file);

        PopToEmptyRam(dest_file);
    }

    else                                                                                                // функция
    {
        fprintf(dest_file, "\t%c инициализация функции '%s'   \n", COMMENT_SYMBOL, prop_name_node->val.prop_name->name);    

        fprintf(dest_file, "%s:\n", init_node->left->left->val.prop_name->name);

        PrintAsmCodeByNode(init_node->left->right, dest_file);

        PrintAsmCodeByNode(init_node->right, dest_file);   

        fprintf(dest_file, "\t%c конец инициализации функции '%s'   \n", COMMENT_SYMBOL, prop_name_node->val.prop_name->name);    
    }
}

void PrintCallAsm(Node *call_node, FILE *dest_file)
{
    assert(call_node);
    assert(dest_file);

    fprintf(stderr, "Start of PrintCallAsm()\n");

    fprintf(dest_file, "\t%c вызов функции '%s'   \n", COMMENT_SYMBOL, call_node->left->left->val.prop_name->name);    

    fprintf(dest_file,  "%s AX   \n", AsmOperations[PUSH_ASM].sym);

    PrintPassArgsInCall(call_node->left->right, dest_file);

    fprintf(dest_file,  "%s BX   \n", AsmOperations[PUSH_ASM].sym);                                                                            // AX := BX
    fprintf(dest_file,  "%s AX   \n", AsmOperations[POP_ASM].sym );

    fprintf(dest_file,  "%s %s: \n", AsmOperations[CALL_ASM].sym, call_node->left->left->val.prop_name->name);  // теперь в стеке лежат BP и res_of_func, их нужно будет свапнуть после выхода из вызова функции

    fprintf(dest_file,  "%s CX  \n", AsmOperations[POP_ASM].sym);                                               // кладём результат функции в CX

    fprintf(dest_file,  "%s AX  \n"     // AX - start of frame, BX - cur size
                        "%s BX  \n"     // BX := AX
                        "%s AX  \n",    // pop AX
    AsmOperations[PUSH_ASM].sym, AsmOperations[POP_ASM].sym, AsmOperations[POP_ASM].sym);

    fprintf(dest_file,  "\t%s CX  \n", AsmOperations[PUSH_ASM].sym);      // результат функции в стек

    fprintf(dest_file, "\t%c конец вызова функции '%s'   \n", COMMENT_SYMBOL, call_node->left->left->val.prop_name->name);    
}

void PrintPassArgsInCall(Node *param_node , FILE *dest_file)
{
    assert(param_node);
    assert(param_node->type == KEY_WORD &&  param_node->val.key_word->name == TREE_COMMA);
    assert(dest_file);

    fprintf(dest_file, "\t%c передача аргументов в функцию \n", COMMENT_SYMBOL);

    if ( param_node ->right != NULL)
        PrintPassArgsInCall( param_node ->right, dest_file);

    PrintAsmCodeByNode( param_node ->left, dest_file);
}

void PrintAssignAsm(Node *assign_node, FILE *dest_file)
{
    assert(assign_node);
    assert(dest_file);
    assert(assign_node->left->type == KEY_WORD && assign_node->left->val.key_word->name == TREE_VAR_T_INDICATOR &&
           assign_node->left->left->type == VAR);

    Node *var_node = assign_node->left->left;

    fprintf(dest_file, "\t%c присваивание значения переменной '%s'\n", COMMENT_SYMBOL, assign_node->left->left->val.prop_name->name);

    PrintAsmCodeByNode(assign_node->right, dest_file);
    fprintf(dest_file, "%s [AX + %lu]\n", AsmOperations[POP_ASM].sym, var_node->val.prop_name->number);

    fprintf(stderr, "var '%s'   num = %lu\n", var_node->val.prop_name->name, var_node->val.prop_name->number);
}

void PrintIfAsm(Node *if_node, FILE *dest_file)
{
    assert(if_node);
    assert(dest_file);
    assert(if_node->left->type == MATH_OP && TREE_NODE_IS_BOOL(if_node->left));

    fprintf(dest_file, "\t%c начало цикла if\n", COMMENT_SYMBOL);

    static size_t if_marks_count = 0;

    size_t cur_mark_num = if_marks_count++;
    PrintMathOpAsm(if_node->left, dest_file);
    fprintf(dest_file, "if_mark_%lu:\n", cur_mark_num);

    PrintAsmCodeByNode(if_node->right, dest_file);

    fprintf(dest_file, "if_mark_%lu:\n", cur_mark_num);

    fprintf(dest_file, "\t%c конец цикла if\n", COMMENT_SYMBOL);
}

void PrintWhileAsm(Node *while_node, FILE *dest_file)
{
    assert(while_node);
    assert(dest_file);
    assert(while_node->left->type == MATH_OP && TREE_NODE_IS_BOOL(while_node->left));

    fprintf(dest_file, "\t%c начало цикла while\n", COMMENT_SYMBOL);

    static size_t while_marks_count = 0;

    size_t cur_mark_num = while_marks_count++;
    fprintf(dest_file, "while_start_mark_%lu:\n", cur_mark_num);

    PrintMathOpAsm(while_node->left, dest_file);
    fprintf(dest_file, "while_end_mark_%lu:\n", cur_mark_num);

    PrintAsmCodeByNode(while_node->right, dest_file);

    fprintf(dest_file,  "%s while_start_mark_%lu:  \n"
                        "while_end_mark_%lu:       \n", AsmOperations[JMP_ASM].sym, cur_mark_num, cur_mark_num);

    fprintf(dest_file, "\t%c конец цикла while\n", COMMENT_SYMBOL);
}

void PrintReturnAsm(Node *ret_node, FILE *dest_file)
{
    assert(ret_node);
    assert(dest_file);

    PrintAsmCodeByNode(ret_node->left, dest_file);

    fprintf(dest_file, "%s\n", AsmOperations[RET_ASM].sym);
}

void PrintSpuInAsm(Node *spu_in_node, FILE *dest_file)
{
    assert(spu_in_node);
    assert(dest_file);

    fprintf(dest_file, "%s\n", AsmOperations[SPU_IN_ASM].sym);
}

void PrintSpuOutAsm(Node *spu_out_node, FILE *dest_file)
{
    assert(spu_out_node);
    assert(dest_file);

    PrintAsmCodeByNode(spu_out_node->left, dest_file);

    fprintf(dest_file, "%s\n", AsmOperations[SPU_OUT_ASM].sym);
}

void PrintAsmCodeByNode(Node *node, FILE *dest_file)
{
    assert(node);
    assert(dest_file);

    if (node->type == NUM)
        fprintf(dest_file, "%s " TREE_ELEM_PRINT_SPECIFIER "\n", AsmOperations[PUSH_ASM].sym, node->val.num);

    if (node->type == MATH_OP)
        PrintMathOpAsm(node, dest_file);
    
    else if (node->type == KEY_WORD)
    {
        KeyWordAsmInfos[node->val.key_word->name].PrintAsmCodeFunc(node, dest_file);
    }

    else if (node->type == NEW_BLOCK)
        PrintAsmCodeByNode(node->left, dest_file);

    else
        PrintChildrenAsm(node, dest_file);
}
