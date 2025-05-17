#include <stdio.h>
#include <assert.h>

#include "front_reader.h"
#include "semantic_anal.h"
#include "tree_lib.h"

void SemanticAnal(Tree *tree)
{
    CheckFuncsValidity(tree, tree->root_ptr);

}

void CheckFuncsValidity(Tree *tree, Node *cur_node)
{
    assert(tree);

    if (cur_node == NULL)
        return;

    if (cur_node->type == NODE_KEY_WORD && cur_node->val.key_word->name == TREE_FUNC_T_INDICATOR)
    {
        ProperName *func_name = cur_node->left->val.prop_name;

        if (!func_name->is_init)
        {
            char error[ERROR_NAME_LEN] = {};
            sprintf(error, "Using undeclared function '%s'", func_name->name);
            SYNTAX_ERROR(tree, cur_node, error);
        }

        size_t argc = GetCountOfArgs(cur_node);
        if (argc != cur_node->left->val.prop_name->args_count)
        {
            char error[ERROR_NAME_LEN] = {};
            sprintf(error, "Incorrect count of arguments in function '%s'", func_name->name);
            SYNTAX_ERROR(tree, cur_node, error);
        }
    }

    else
    {
        CheckFuncsValidity(tree, cur_node->left);
        CheckFuncsValidity(tree, cur_node->right);
    }
}

// void SyntaxError(Tree *tree, Node *cur_node, const char *error, const char *file, int line, const char *func)
// {
//     assert(tree);
//     assert(cur_node);
//     assert(error);
//     assert(file);
//     assert(func);

//     TREE_DUMP(tree);

//     fprintf(stderr, "SyntaxError called in %s:%d %s()\n"
//                     "Error: %s   ( position %lu:%lu )",
//                     file, line, func, error, cur_node->born_line, cur_node->born_column);

//                     // Syntax error: forgot to put ) here (file ...,line ...)   // TODO ??
//                         // int main (int argc, char *argv[]   {
//                                                         //  ^ 
//     abort();
// }