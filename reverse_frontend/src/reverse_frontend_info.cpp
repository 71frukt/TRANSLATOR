#include <stdio.h>
#include <assert.h>

#include "reverse_frontend_info.h"


void PrintNodeMySym(Node *node, FILE *dest_file)
{
    assert(dest_file);

    if (node == NULL)
        return;

    else if (node->type == NEW_BLOCK)
    {
        fprintf(dest_file, "%s \n", Managers[OPEN_BLOCK_BRACKET].my_symbol);
        PrintNodeMySym(node->left, dest_file);
        fprintf(dest_file, "%s \n", Managers[CLOSE_BLOCK_BRACKET].my_symbol);

        PrintNodeMySym(node->right, dest_file);
    }

    else if (node->type == MATH_OP)
    {
        if (node->val.math_op->type == UNARY)
        {
            fprintf(dest_file, "%s ", node->val.math_op->my_symbol);
            PrintNodeMySym(node->left, dest_file);
        }

        else
        {
            PrintNodeMySym(node->left, dest_file);
            fprintf(dest_file, "%s ", node->val.math_op->my_symbol);
            PrintNodeMySym(node->right, dest_file);
        }
    }

    else if (node->type == VAR || node->type == FUNC)
        fprintf(dest_file, "%s ", node->val.prop_name->name);
    
    else if (node->type == NUM)
        fprintf(dest_file, TREE_ELEM_PRINT_SPECIFIER " ", node->val.num);

    else if (node->type == KEY_WORD)
    {
        KeyWordReverseFrontendInfos[node->val.key_word->name].reverse_frontend_func(node, dest_file);
    }
}


void ReverseFrontVarT(Node *var_t_node, FILE *dest_file)
{
    fprintf(dest_file, "%s ", var_t_node->left->val.prop_name->name);
}

void ReverseFrontFuncT(Node *func_t_node, FILE *dest_file)
{
    fprintf(dest_file, "%s ", func_t_node->left->val.prop_name->name);

    fprintf(dest_file, "%s ", Managers[OPEN_EXPR_BRACKET].my_symbol);
    ReverseFrontendComma(func_t_node->right, dest_file);
    fprintf(dest_file, "%s \n", Managers[CLOSE_EXPR_BRACKET].my_symbol);
}

void ReverseFrontInit(Node *init_node, FILE *dest_file)
{
    fprintf(dest_file, "%s ", init_node->val.key_word->my_symbol);

    PrintNodeMySym(init_node->left, dest_file);

    if (init_node->left->type == KEY_WORD && init_node->left->val.key_word->name == TREE_VAR_T_INDICATOR && init_node->right != NULL)
        fprintf(dest_file, "%s ", KeyWords[TREE_ASSIGN].my_symbol);

    if (init_node->right != NULL)
    PrintNodeMySym(init_node->right, dest_file);
}

void ReverseFrontNewExpr(Node *new_expr_node, FILE *dest_file)
{
    PrintNodeMySym(new_expr_node->left, dest_file);

    if (new_expr_node->left->type != KEY_WORD || (new_expr_node->left->val.key_word->name != TREE_IF && new_expr_node->left->val.key_word->name != TREE_WHILE))  // для if и while эта нода искусственная, её в исходном коде нет
        fprintf(dest_file, "%s\n", KeyWords[TREE_NEW_EXPR].my_symbol);
    PrintNodeMySym(new_expr_node->right, dest_file);
}

void ReverseFrontendComma(Node *cur_comma, FILE *dest_file)
{
    PrintNodeMySym(cur_comma->left, dest_file);

    if (cur_comma->right != NULL)
    {
        fprintf(dest_file, "%s ", KeyWords[TREE_COMMA].my_symbol);
        ReverseFrontendComma(cur_comma->right, dest_file);
    }
}

void ReverseFrontAssign(Node *assign_node, FILE *dest_file)
{
    PrintNodeMySym(assign_node->left,  dest_file);

    fprintf(dest_file, "%s ", KeyWords[TREE_ASSIGN].my_symbol);

    PrintNodeMySym(assign_node->right, dest_file);
}

void ReverseFrontIfWhile(Node *cur_node, FILE *dest_file)
{
    fprintf(dest_file, "%s ", KeyWords[TREE_IF].my_symbol);

    fprintf(dest_file, "%s ", Managers[OPEN_EXPR_BRACKET].my_symbol);
    PrintNodeMySym(cur_node->left, dest_file);
    fprintf(dest_file, "%s \n", Managers[CLOSE_EXPR_BRACKET].my_symbol);

    PrintNodeMySym(cur_node->right, dest_file);
}

void ReverseFrontReturn(Node *cur_node, FILE *dest_file)
{
    fprintf(dest_file, "%s ", cur_node->val.key_word->my_symbol);
    PrintNodeMySym(cur_node->left, dest_file);
}

void ReverseFrontSpuIn(Node *cur_node, FILE *dest_file)
{
    fprintf(dest_file, "%s ", cur_node->val.key_word->my_symbol);
    PrintNodeMySym(cur_node->left, dest_file);
}

void ReverseFrontSpuOut(Node *cur_node, FILE *dest_file)
{
    fprintf(dest_file, "%s ", cur_node->val.key_word->my_symbol);
    PrintNodeMySym(cur_node->left, dest_file);
}

void ReverseFrontChildren(Node *node, FILE *dest_file)
{
    PrintNodeMySym(node->left,  dest_file);
    PrintNodeMySym(node->right, dest_file);
}