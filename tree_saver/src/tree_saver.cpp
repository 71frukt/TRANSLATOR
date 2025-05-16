#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "tree_saver.h"

FILE *GetInputSaveFile(const int argc, const char *argv[])
{
    FILE *input_file = NULL;

    if (argc < 2)
        input_file = fopen(BASE_SAVE_TREE_FILE_NAME, "r");
    
    else
        input_file = fopen(argv[1], "r");

    return input_file;   
}

FILE *GetOutputSaveFile(const int argc, const char *argv[])
{
    if (argc < 3)
        return fopen(BASE_SAVE_TREE_FILE_NAME, "w");
    
    else
        return fopen(argv[2], "w");
}

void WriteTreeData(Node *cur_node, FILE *dest_file)
{
    fprintf(dest_file, "%s ", NodeValToStr(cur_node));
    fprintf(stderr, "write int file  '%s' \n", NodeValToStr(cur_node));

    if (cur_node != NULL)
    {
        fprintf(dest_file, "( ");

        WriteTreeData(cur_node->left,  dest_file);

        fprintf(dest_file, "; ");

        WriteTreeData(cur_node->right, dest_file);

        fprintf(dest_file, ") ");
    }
}

void BuildTreeByFileData(FILE *source, Tree *tree)
{
    assert(source);
    assert(tree);

    tree->root_ptr = MakeAndLinkNodes(source, tree);

    TREE_DUMP(tree);

    MakeNamesTablesForBlocks(tree, tree->root_ptr);
}

Node *MakeAndLinkNodes(FILE *source, Tree *tree)
{
    char cur_token[TOKEN_LEN] = {};

    if (fscanf(source, "%s", cur_token) == 0 || strcmp(cur_token, "NULL") == 0)
        return NULL;

// fprintf(stderr, "token = '%s'\n", cur_token);

    Node *new_node = NewNode(tree, POISON_TYPE, {}, NULL, NULL);

    GetNodeInfoBySymbol(cur_token, tree, new_node, REAL_CODE_MODE);

    if (fscanf(source, "%s", cur_token) != 1 || strcmp(cur_token, "(") != 0)
        fprintf(stderr, "Expected '(' after '%s'\n", cur_token);

    new_node->left = MakeAndLinkNodes(source, tree);

    if (fscanf(source, "%s", cur_token) != 1 || strcmp(cur_token, ";") != 0)
        fprintf(stderr, "Expected ';' after '%s'\n", cur_token);

    new_node->right = MakeAndLinkNodes(source, tree);

    if (fscanf(source, "%s", cur_token) != 1 || strcmp(cur_token, ")") != 0)
        fprintf(stderr, "Expected ')' after '%s'\n", cur_token);

    if (new_node->type == KEY_WORD && new_node->val.key_word->name == TREE_FUNC_T_INDICATOR)
        new_node->left->type = FUNC;
    
    else if (new_node->type == KEY_WORD && new_node->val.key_word->name == TREE_VAR_T_INDICATOR)
        new_node->left->type = VAR;

    return new_node;
}