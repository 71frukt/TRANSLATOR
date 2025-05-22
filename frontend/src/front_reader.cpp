#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <string.h>

#include "tree_lib.h"
#include "front_reader.h"
#include "semantic_anal.h"

FILE *GetInputCodeFile(const int argc, const char *argv[])
{
    FILE *input_file = NULL;

    if (argc < 2)
        input_file = fopen(BASE_INPUT_CODE_FILE_NAME, "r");
    
    else
        input_file = fopen(argv[1], "r");

    return input_file;   
}

void BuildTreeByCode(Tree *tree, FILE *source)
{
    MakeTokens(tree, source);

    tree->root_ptr = GetCode(tree);

    MakeNamesTablesForBlocks(tree, tree->root_ptr);

    TREE_DUMP(tree);
}

void MakeTokens(Tree *tree, FILE *source)
{
    size_t cur_line   = 0;
    size_t cur_column = 0;

    char cur_ch = (char) getc(source);
    ungetc(cur_ch, source);

    while (cur_ch != EOF)
    {
        cur_ch = (char) getc(source);

        if (cur_ch == EOF)
            break;
        
        if (cur_ch == '\n')
        {
            cur_line++;
            cur_column = 0;
            continue;
        }

        else if (isspace(cur_ch))
        {
            cur_column++;
            continue;
        }

        if (isdigit(cur_ch))
        {
            ungetc(cur_ch, source);
            int shift = 0;

            TreeElem_t val = POISON_VAL;

            fscanf(source, TREE_ELEM_SCANF_SPECIFIER "%n", &val, &shift);
            cur_column += (size_t) shift;

            Node *new_node = NewNode(tree, NODE_NUM, {.num = val}, NULL, NULL);
            new_node->born_column = cur_column;
            new_node->born_line   = cur_line;
        }

        else
        {
            char cur_token[TOKEN_STR_LEN] = {};

            ungetc(cur_ch, source);
            int shift = 0;

            fscanf(source, "%s%n", cur_token, &shift);
            cur_column += (size_t) shift;

            Node *new_node = GetNamedToken(tree, cur_token);

            if (new_node != NULL)
            {
                new_node->born_column = cur_column;
                new_node->born_line   = cur_line;
            }
        }
    }

    NewNode(tree, NODE_MANAGER, {.manager = &Managers[EOT]}, NULL, NULL);
    TREE_DUMP(tree);
}

Node *GetNamedToken(Tree *tree, char *token_name)
{
    if (IsEngLetter(*token_name))
    {
        ProperName *cur_name_ptr = FindNameInTable(&tree->names_table, token_name);

        if (cur_name_ptr == NULL)
        {
            cur_name_ptr = NewNameInTable(&tree->names_table, token_name);
        }

        return NewNode(tree, NODE_VAR_OR_FUNC, {.prop_name = cur_name_ptr}, NULL, NULL);
    }

    else
    {
        const MathOperation   *cur_op     = GetOperationBySymbol  (token_name, MY_CODE_MODE);
        const KeyWord         *key_word   = GetKeyWordBySymbol    (token_name, MY_CODE_MODE);
        const ManageElem      *manage_el  = GetManageElemBySymbol (token_name, MY_CODE_MODE);

        if (cur_op != NULL)
        {
            return NewNode(tree, NODE_MATH_OP, {.math_op = cur_op}, NULL, NULL);
        }

        else if (key_word != NULL)
        {
            return NewNode(tree, NODE_KEY_WORD, {.key_word = key_word}, NULL, NULL);
        }

        else if (manage_el != NULL)
        {
            return NewNode(tree, NODE_MANAGER, {.manager = manage_el}, NULL, NULL);
        }

        // Node *new_node = NewNode(tree, NODE_POISON_TYPE, {}, NULL, NULL);
        // return GetNodeInfoBySymbol(token_name, tree, new_node, MY_CODE_MODE);
    }

    return NULL;
}

bool IsEngLetter(char ch) // isalpha
{
    return ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'));
}

bool IsCapitalLetter(char letter)
{
    return (letter >= 'À' && letter <= 'ß');
}

Node *GetCode(Tree *dest_tree)
{
    assert(dest_tree);

    size_t ip = 0;

    Node **tokens  = dest_tree->node_ptrs;
    Node *res_code = GetFuncInit(dest_tree, &ip);
    Node *cur_node = res_code;

    while ( !(tokens[ip]->type == NODE_MANAGER && tokens[ip]->val.manager->name == EOT) )
    {
        Node *new_func = GetFuncInit(dest_tree, &ip);

        cur_node->right = new_func;
        cur_node = new_func;
    }
    
    if (tokens[ip]->type != NODE_MANAGER || tokens[ip]->val.manager->name != EOT)
        SYNTAX_ERROR(dest_tree, tokens[ip], "EOT");

    RemoveNode(dest_tree, &tokens[ip]);

    return res_code;    
}

Node *GetFuncInit(Tree *dest_tree, size_t *ip)
{
    assert(dest_tree);
    assert(ip);
    TREE_DUMP(dest_tree);

    Node **tokens = dest_tree->node_ptrs;

    if (!TREE_NODE_IS_INIT(tokens[*ip]) || tokens[*ip + 1]->type != NODE_VAR_OR_FUNC)
        SYNTAX_ERROR(dest_tree, tokens[*ip], "init of func");
  
    Node *func_init = tokens[(*ip)++];
    Node *func_node = tokens[(*ip)++];
    func_node->type = NODE_FUNC;

    if (tokens[*ip]->type != NODE_MANAGER || tokens[*ip]->val.manager->name != OPEN_EXPR_BRACKET)
        SYNTAX_ERROR(dest_tree, tokens[*ip], "open bracket in declaring arguments");

    RemoveNode(dest_tree, &tokens[(*ip)++]);

    if (!TREE_NODE_IS_INIT(tokens[*ip]) || tokens[*ip + 1]->type != NODE_VAR_OR_FUNC)
        SYNTAX_ERROR(dest_tree, tokens[*ip], "init of arguments");

    Node *arg = GetExprSequence(dest_tree, ip);

    func_init->left = NewNode(dest_tree, NODE_KEY_WORD, {.key_word = &KeyWords[TREE_FUNC_T_INDICATOR]}, func_node, arg);

    if (tokens[*ip]->type != NODE_MANAGER || tokens[*ip]->val.manager->name != CLOSE_EXPR_BRACKET)
        SYNTAX_ERROR(dest_tree, tokens[*ip], "close bracket in declaring arguments");

    RemoveNode(dest_tree, &tokens[(*ip)++]);

    func_init->right = GetBlock(dest_tree, ip);

    return NewNode(dest_tree, NODE_KEY_WORD, {.key_word = &KeyWords[TREE_NEW_FUNC]}, func_init, NULL);
}

Node *GetBlock(Tree *dest_tree, size_t *ip)
{
    assert(dest_tree);
    assert(ip);

    Node **tokens = dest_tree->node_ptrs;

    if (tokens[*ip]->type != NODE_MANAGER || tokens[*ip]->val.manager->name != OPEN_BLOCK_BRACKET)
        SYNTAX_ERROR(dest_tree, tokens[*ip], "open block bracket");

    RemoveNode(dest_tree, &tokens[(*ip)++]);

    Node *res_block = GetIf(dest_tree, ip);
    Node *cur_top = res_block;

    while (tokens[*ip]->type != NODE_MANAGER || tokens[*ip]->val.manager->name != CLOSE_BLOCK_BRACKET)
    {
        Node *new_top = GetIf(dest_tree, ip);

        cur_top->right = new_top;
        cur_top = new_top;
    }

    Node *new_block_node = NewNode(dest_tree, NODE_NEW_BLOCK, {.block = {}}, res_block, NULL);
    RemoveNode(dest_tree, &tokens[(*ip)++]);

    return new_block_node;
}

Node *GetIf(Tree *dest_tree, size_t *ip)
{
    assert(dest_tree);
    assert(ip);

    Node **tokens = dest_tree->node_ptrs;

    if (tokens[*ip]->type != NODE_KEY_WORD || tokens[*ip]->val.key_word->name != TREE_IF)
        return GetWhile(dest_tree, ip);

    Node *if_node = tokens[(*ip)++];

    if (tokens[*ip]->type != NODE_MANAGER || tokens[*ip]->val.manager->name != OPEN_EXPR_BRACKET)
        SYNTAX_ERROR(dest_tree, tokens[*ip], "open bracket for condition");
    RemoveNode(dest_tree, &tokens[(*ip)++]);

    if_node->left = GetBool(dest_tree, ip);

    if (tokens[*ip]->type != NODE_MANAGER || tokens[*ip]->val.manager->name != CLOSE_EXPR_BRACKET)
        SYNTAX_ERROR(dest_tree, tokens[*ip], "close bracket for condition");
    RemoveNode(dest_tree, &tokens[(*ip)++]);

    if_node->right = GetBlock(dest_tree, ip);

    return NewNode(dest_tree, NODE_KEY_WORD, {.key_word = &KeyWords[TREE_NEW_EXPR]}, if_node, NULL);
}

Node *GetWhile(Tree *dest_tree, size_t *ip)
{
    assert(dest_tree);
    assert(ip);

    Node **tokens = dest_tree->node_ptrs;

    if (tokens[*ip]->type != NODE_KEY_WORD || tokens[*ip]->val.key_word->name != TREE_WHILE)
        return GetExpr(dest_tree, ip);

    Node *while_node = tokens[(*ip)++];

    if (tokens[*ip]->type != NODE_MANAGER || tokens[*ip]->val.manager->name != OPEN_EXPR_BRACKET)
        SYNTAX_ERROR(dest_tree, tokens[*ip], "open bracket for condition");
    RemoveNode(dest_tree, &tokens[(*ip)++]);

    while_node->left = GetBool(dest_tree, ip);

    if (tokens[*ip]->type != NODE_MANAGER || tokens[*ip]->val.manager->name != CLOSE_EXPR_BRACKET)
        SYNTAX_ERROR(dest_tree, tokens[*ip], "close bracket for condition");
    RemoveNode(dest_tree, &tokens[(*ip)++]);

    while_node->right = GetBlock(dest_tree, ip);

    return NewNode(dest_tree, NODE_KEY_WORD, {.key_word = &KeyWords[TREE_NEW_EXPR]}, while_node, NULL);
}

Node *GetExpr(Tree *dest_tree, size_t *ip)
{
    assert(dest_tree);
    assert(ip);

    Node **tokens = dest_tree->node_ptrs;

    Node *expr = GetVarInit(dest_tree, ip);

    if (tokens[*ip]->type != NODE_KEY_WORD || tokens[*ip]->val.key_word->name != TREE_NEW_EXPR)
        SYNTAX_ERROR(dest_tree, tokens[*ip], "end of expr my_symbol");

    Node *res_line = tokens[(*ip)++];
    res_line->left = expr;

    return res_line;
}

Node *GetVarInit(Tree *dest_tree, size_t *ip)
{
    assert(dest_tree);
    assert(ip);
    
    Node **tokens = dest_tree->node_ptrs;

    if (!TREE_NODE_IS_INIT(tokens[*ip]))
        return GetReturn(dest_tree, ip);

    Node *init_node = tokens[(*ip)++];
    Node *var_node  = GetVarOrFunc(dest_tree, ip);

    if (var_node->type != NODE_KEY_WORD || var_node->val.key_word->name != TREE_VAR_T_INDICATOR)
        SYNTAX_ERROR(dest_tree, tokens[*ip], "type of NODE_VAR");

    init_node->left = var_node;

    if (!(tokens[*ip]->type == NODE_KEY_WORD && tokens[*ip]->val.key_word->name == TREE_ASSIGN))
        return init_node;

    RemoveNode(dest_tree, &tokens[(*ip)++]);

    init_node->right = GetBool(dest_tree, ip);
    
    return init_node;
}

Node *GetExprSequence(Tree *dest_tree, size_t *ip)
{
    assert(dest_tree);
    assert(ip);

    Node **tokens   = dest_tree->node_ptrs;
    Node *arg       = GetVarInit(dest_tree, ip);
    Node *cur_comma = NewNode(dest_tree, NODE_KEY_WORD, {.key_word = &KeyWords[TREE_COMMA]}, arg, NULL);
    Node *res_comma = cur_comma;
TREE_DUMP(dest_tree);
    while (tokens[*ip]->type == NODE_KEY_WORD && tokens[*ip]->val.key_word->name == TREE_COMMA)
    {
        Node *new_comma = tokens[(*ip)++];
        Node *new_arg = GetVarInit(dest_tree, ip);

        cur_comma->right = new_comma;
        cur_comma = new_comma;
        
        new_comma->left = new_arg;
    }

    return res_comma;
}

Node *GetReturn(Tree *dest_tree, size_t *ip)
{
    assert(dest_tree);
    assert(ip);

    Node **tokens = dest_tree->node_ptrs;

    if (tokens[*ip]->type != NODE_KEY_WORD || tokens[*ip]->val.key_word->name != TREE_RETURN)
        return GetAssign(dest_tree, ip);

    Node *ret_node = tokens[(*ip)++];
    ret_node->left = GetBool(dest_tree, ip);

    return ret_node;
}

Node *GetAssign(Tree *dest_tree, size_t *ip)
{
    assert(dest_tree);
    assert(ip);

    Node **tokens = dest_tree->node_ptrs;

    if (tokens[(*ip) + 1]->type != NODE_KEY_WORD || tokens[(*ip) + 1]->val.key_word->name != TREE_ASSIGN)
        return GetBool(dest_tree, ip);

    Node *var_node  = GetVarOrFunc(dest_tree, ip);

    if (var_node->type != NODE_KEY_WORD || var_node->val.key_word->name != TREE_VAR_T_INDICATOR)
        SYNTAX_ERROR(dest_tree, var_node, "an attempt to assign a non-variable");

    Node *init_node = tokens[(*ip)++];
    Node *expr_node = GetBool(dest_tree, ip);

    init_node->left  = var_node;
    init_node->right = expr_node;

    return init_node;
}

Node *GetBool(Tree *dest_tree, size_t *ip)
{
    assert(dest_tree);
    assert(ip);

    Node **tokens  = dest_tree->node_ptrs;
    Node *res_node = GetSum(dest_tree, ip);

    while (tokens[*ip]->type == NODE_MATH_OP && TREE_NODE_IS_BOOL(tokens[*ip]))
    {
        Node *arg_1 = res_node;
        res_node    = tokens[(*ip)++];
        Node *arg_2 = GetSum(dest_tree, ip);

        res_node->left  = arg_1;
        res_node->right = arg_2;
    }

    return res_node;
}

Node *GetSum(Tree *dest_tree, size_t *ip)
{
    assert(dest_tree);
    assert(ip);

    Node **tokens  = dest_tree->node_ptrs;
    Node *res_node = GetMul(dest_tree, ip);

    while (tokens[*ip]->type == NODE_MATH_OP && (tokens[*ip]->val.math_op->num == TREE_ADD || tokens[*ip]->val.math_op->num == TREE_SUB))
    {
        Node *arg_1 = res_node;
        res_node    = tokens[(*ip)++];
        Node *arg_2 = GetMul(dest_tree, ip);

        res_node->left  = arg_1;
        res_node->right = arg_2;
    }

    return res_node;
}

Node *GetMul(Tree *dest_tree, size_t *ip)
{
    assert(dest_tree);
    assert(ip);

    Node **tokens  = dest_tree->node_ptrs;
    Node *res_node = GetPow(dest_tree, ip);

    while (tokens[*ip]->type == NODE_MATH_OP && (tokens[*ip]->val.math_op->num == TREE_MUL || tokens[*ip]->val.math_op->num == TREE_DIV))
    {
        Node *arg_1 = res_node;
        res_node    = tokens[(*ip)++];
        Node *arg_2 = GetPow(dest_tree, ip);

        res_node->left  = arg_1;
        res_node->right = arg_2;
    }

    return res_node;
}

Node *GetPow(Tree *dest_tree, size_t *ip)
{
    assert(dest_tree);
    assert(ip);

    Node **tokens  = dest_tree->node_ptrs;
    Node *res_node = GetOp(dest_tree, ip);

    Node *cur_op = tokens[*ip];

    if (cur_op->type != NODE_MATH_OP)
        return res_node;

    if (cur_op->val.math_op->num == TREE_DEG)
    {
        Node *basis = res_node;
        res_node = tokens[(*ip)++];
        Node *degree = GetPow(dest_tree, ip);
        
        res_node->left  = basis;
        res_node->right = degree;

        return res_node;
    }

    else
        return res_node;
}

Node *GetOp(Tree *dest_tree, size_t *ip)              // f(..) ï¿½ï¿½ï¿½ f(.. , ..)
{
    assert(dest_tree);
    assert(ip);

    Node **tokens = dest_tree->node_ptrs;

    if (tokens[*ip]->type != NODE_MATH_OP)
        return GetSumInBrackets(dest_tree, ip);


    Node *op_node = tokens[(*ip)++];

    if (op_node->val.math_op->form == INFIX)
        SYNTAX_ERROR(dest_tree, op_node, "expected prefix form of operand");

    if (op_node->val.math_op->type == UNARY)
    {
        Node *arg = GetSumInBrackets(dest_tree, ip);
        op_node->left  = arg;
        op_node->right = NULL;

        return op_node;
    }

    else
    {
        bool is_open_expr_bracket = tokens[*ip]->type == NODE_MANAGER && tokens[*ip]->val.manager->name == OPEN_EXPR_BRACKET;
        if (!is_open_expr_bracket)
            SYNTAX_ERROR(dest_tree, tokens[*ip], Managers[OPEN_EXPR_BRACKET].my_symbol);
        RemoveNode(dest_tree, &tokens[*ip]);
        (*ip)++;

        Node *arg_1 = GetSum(dest_tree, ip);

        bool is_comma = tokens[*ip]->type == NODE_KEY_WORD && tokens[*ip]->val.key_word->name == TREE_COMMA;
        if (!is_comma)
            SYNTAX_ERROR(dest_tree, tokens[*ip], Managers[TREE_COMMA].my_symbol);
        RemoveNode(dest_tree, &tokens[*ip]);
        (*ip)++;

        Node *arg_2 = GetSum(dest_tree, ip);

        bool is_close_expr_bracket = tokens[*ip]->type == NODE_MANAGER && tokens[*ip]->val.manager->name == CLOSE_EXPR_BRACKET;
        if (!is_close_expr_bracket)
            SYNTAX_ERROR(dest_tree, tokens[*ip], Managers[CLOSE_EXPR_BRACKET].my_symbol);
        RemoveNode(dest_tree, &tokens[*ip]);
        (*ip)++;

        op_node->left  = arg_1;
        op_node->right = arg_2;

        return op_node;       
    }
}

Node *GetSumInBrackets(Tree *dest_tree, size_t *ip)
{
    assert(dest_tree);
    assert(ip);

    Node **tokens = dest_tree->node_ptrs;

    if (tokens[*ip]->type == NODE_MANAGER && tokens[*ip]->val.manager->name == OPEN_EXPR_BRACKET)
    {
        RemoveNode(dest_tree, &tokens[*ip]);
        (*ip)++;

        Node *res_node = GetSum(dest_tree, ip);

        bool is_close_expr_bracket = tokens[*ip]->type == NODE_MANAGER && tokens[*ip]->val.manager->name == CLOSE_EXPR_BRACKET;

        if (!is_close_expr_bracket)
            SYNTAX_ERROR(dest_tree, tokens[*ip], Managers[CLOSE_EXPR_BRACKET].my_symbol);

        RemoveNode(dest_tree, &tokens[*ip]);
        (*ip)++;
        
        return res_node;
    }
    
    else
        return GetVarOrFunc(dest_tree, ip);
}

Node *GetVarOrFunc(Tree *dest_tree, size_t *ip)
{
    assert(dest_tree);
    assert(ip);

    Node **tokens = dest_tree->node_ptrs;

    if (tokens[*ip]->type == NODE_VAR_OR_FUNC)
    {

        Node *cur_node = tokens[(*ip)++];
        cur_node->type = NODE_VAR;
        Node *arg = NULL;

        if (tokens[*ip]->type == NODE_MANAGER && tokens[*ip]->val.manager->name == OPEN_EXPR_BRACKET)
        {
            RemoveNode(dest_tree, &tokens[(*ip)++]);

            cur_node->type = NODE_FUNC;
            arg = GetExprSequence(dest_tree, ip);

            if (tokens[*ip]->type != NODE_MANAGER || tokens[*ip]->val.manager->name != CLOSE_EXPR_BRACKET)
                SYNTAX_ERROR(dest_tree, tokens[*ip], Managers[CLOSE_EXPR_BRACKET].my_symbol);

            RemoveNode(dest_tree, &tokens[(*ip)++]);

            Node *new_func = NewNode(dest_tree, NODE_KEY_WORD, {.key_word = &KeyWords[TREE_FUNC_T_INDICATOR]}, cur_node, arg);

            return NewNode(dest_tree, NODE_KEY_WORD, {.key_word = &KeyWords[TREE_FUNC_CALL]}, new_func, NULL);
        }

        else
        {
            return NewNode(dest_tree, NODE_KEY_WORD, {.key_word = &KeyWords[TREE_VAR_T_INDICATOR]}, cur_node, arg);
        }
    }

    else
        return GetScanf(dest_tree, ip);
}

Node *GetScanf(Tree *dest_tree, size_t *ip)
{
    assert(dest_tree);
    assert(ip);

    Node **tokens = dest_tree->node_ptrs;

    if (tokens[*ip]->type == NODE_KEY_WORD && tokens[*ip]->val.key_word->name == TREE_SPU_IN)
        return tokens[(*ip)++];

    else 
        return GetPrintf(dest_tree, ip);
}

Node *GetPrintf(Tree *dest_tree, size_t *ip)
{
    assert(dest_tree);
    assert(ip);

    Node **tokens = dest_tree->node_ptrs;

    if (tokens[*ip]->type == NODE_KEY_WORD && tokens[*ip]->val.key_word->name == TREE_SPU_OUT)
    {
        Node *spu_out_node = tokens[(*ip)++];

        spu_out_node->left = GetSum(dest_tree, ip);

        return spu_out_node;
    }

    else
        return GetNumber(dest_tree, ip);
}

Node *GetNumber(Tree *dest_tree, size_t *ip)
{
    assert(dest_tree);
    assert(ip);

    Node **tokens = dest_tree->node_ptrs;

    if (tokens[*ip]->type == NODE_NUM)
    {
        return tokens[(*ip)++];
    }

    else
    {
        SYNTAX_ERROR(dest_tree, tokens[*ip], "type of NODE_NUM or NODE_VAR");
        return NULL;
    }
}