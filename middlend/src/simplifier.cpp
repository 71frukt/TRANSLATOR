#include <stdio.h>
#include <assert.h>

#include "simplifier.h"

void SimplifyTree(Tree *code_tree)
{
    TREE_DUMP(code_tree);

    bool was_simplified = false;

    do
    {
        was_simplified = false;
        fprintf(stderr, "simplify\n");
        SimplifyConstants  (code_tree, code_tree->root_ptr, &was_simplified);
        // DelNeutralElements (code_tree, code_tree->root_ptr, &was_simplified);
    }
    while (was_simplified);

    TREE_DUMP(code_tree);
}

Node *SimplifyConstants(Tree *code_tree, Node *cur_node, bool *was_simplified)
{
    assert(code_tree);
    assert(was_simplified);

    if (cur_node == NULL || cur_node->type == NUM || cur_node->type == VAR)
        return cur_node;

    cur_node->left  = SimplifyConstants(code_tree, cur_node->left,  was_simplified);
    cur_node->right = SimplifyConstants(code_tree, cur_node->right, was_simplified);

    
    if (cur_node->type == MATH_OP)
    {
        bool args_are_num = (cur_node->left->type == NUM && (cur_node->val.math_op->type == UNARY || cur_node->right->type == NUM));

        if (args_are_num)
        {
            *was_simplified = true;
            CalculateNode(code_tree, cur_node);
            
        }
    }

    return cur_node;
}

Node *CalculateNode(Tree *tree, Node *op_node)
{
    assert(tree);
    assert(op_node);
    assert(op_node->type == MATH_OP);
    assert(op_node->left != NULL && (op_node->val.math_op->type == UNARY || op_node->right != NULL));

    if (op_node->left->type != NUM || op_node->right->type != NUM)
    {
        fprintf(stderr, "Trying to calculate op with not-a-number args\n");
        return op_node;
    }

    assert(op_node->left);
    assert(op_node->right);

    TreeElem_t res_val = op_node->val.math_op->op_func(op_node->left, op_node->right);

    RemoveNode(tree, &op_node->left);
    RemoveNode(tree, &op_node->right);

    op_node->type = NUM;
    op_node->val.num = res_val;

    return op_node;
}

Node *DelNeutralElements(Tree *code_tree, Node *cur_node, bool *was_simplified)
{
    assert(code_tree);
    assert(was_simplified);

    if (cur_node == NULL || cur_node->type == NUM || cur_node->type == VAR)
        return cur_node;

    cur_node->left  = DelNeutralElements(code_tree, cur_node->left,  was_simplified);
    cur_node->right = DelNeutralElements(code_tree, cur_node->right, was_simplified);

    if (cur_node->type == MATH_OP)
    {
        MathOpSimplifierInfos[cur_node->val.math_op->num].simpl_vars_func(code_tree, cur_node, was_simplified);
    }

    else if (cur_node->type == KEY_WORD)
    {
        if (cur_node->val.key_word->name == TREE_IF || cur_node->val.key_word->name == TREE_WHILE)
        {
            if (cur_node->left->type == NUM && cur_node->left->val.num == 0)
            {
                RemoveSubtree(code_tree, &cur_node);
                return NULL;
            }

            else if (cur_node->left->type == NUM && cur_node->left->val.num != 0)
            {
                if (cur_node->val.key_word->name == TREE_WHILE)
                    return cur_node;

                RemoveSubtree (code_tree, &cur_node->left);

                Node *block_node = cur_node->right;
                *cur_node = *block_node;

                RemoveNode  (code_tree, &block_node);
            }
        }
    }

    return cur_node;
}

Node *SimpleBoolEqVars(Tree *tree, Node *cur_node, bool *was_simplified)      // ! assert'ы перед вызовом функций
{
    Node *arg_1 = cur_node->left;
    Node *arg_2 = cur_node->right;

    assert(arg_1);
    assert(arg_2);

    if (arg_1->type == VAR && arg_2->type == VAR &&
        arg_1->val.prop_name == arg_2->val.prop_name)
    {
        RemoveNode(tree, &cur_node->left);
        RemoveNode(tree, &cur_node->right);

        cur_node->type = NUM;
        cur_node->val.num = 1;

        *was_simplified = true;
    }

    return cur_node;
}

Node *SimpleAddVars(Tree *tree, Node *cur_node, bool *was_simplified)
{
    Node *arg_1 = cur_node->left;
    Node *arg_2 = cur_node->right;

    assert(arg_1);
    assert(arg_2);

    if (arg_1->type == NUM && arg_1->val.num == 0)          // 0 + f(x) --> f(x)
    {
        *cur_node = *arg_2;

        RemoveNode(tree, &arg_1);
        RemoveNode(tree, &arg_2);

        *was_simplified = true;
    }

    else if (arg_2->type == NUM && arg_2->val.num == 0)     // f(x) + 0 --> f(x)
    {
        *cur_node = *arg_1;

        RemoveNode(tree, &arg_1);
        RemoveNode(tree, &arg_2);

        *was_simplified = true;
    }

    return cur_node;
}

Node *SimpleSubVars(Tree *tree, Node *cur_node, bool *was_simplified)
{
    Node *arg_1 = cur_node->left;
    Node *arg_2 = cur_node->right;

    assert(arg_1);
    assert(arg_2);

    if (arg_2->type == NUM && arg_2->val.num == 0)      // f(x) - 0 --> f(x)
    {
        *cur_node = *arg_1;

        RemoveNode(tree, &arg_1);
        RemoveNode(tree, &arg_2);    

        *was_simplified = true;    
    }

    return cur_node;
}

Node *SimpleMulVars(Tree *tree, Node *cur_node, bool *was_simplified)
{
    Node *arg_1 = cur_node->left;
    Node *arg_2 = cur_node->right;

    assert(arg_1);
    assert(arg_2);

    if ((arg_1->type == NUM && arg_1->val.num == 0) || (arg_2->type == NUM && arg_2->val.num == 0))          // 0 * f(x)  || f(x) * 0 --> 0
    {
        cur_node->type    = NUM;
        cur_node->val.num = 0;
        cur_node->left    = NULL;
        cur_node->right   = NULL;

        RemoveSubtree(tree, &arg_1);
        RemoveSubtree(tree, &arg_2);       

        *was_simplified = true; 
    }

    else if (arg_1->type == NUM && arg_1->val.num == 1)          // 1 * f(x) --> f(x)
    {
        *cur_node = *arg_2;

        RemoveNode(tree, &arg_1);
        RemoveNode(tree, &arg_2);        

        *was_simplified = true;
    }

    else if (arg_2->type == NUM && arg_2->val.num == 1)     // f(x) * 1 --> f(x)
    {
        *cur_node = *arg_1;

        RemoveNode(tree, &arg_1);
        RemoveNode(tree, &arg_2);      

        *was_simplified = true;  
    }

    return cur_node;
}

Node *SimpleDivVars(Tree *tree, Node *cur_node, bool *was_simplified)
{
    Node *arg_1 = cur_node->left;
    Node *arg_2 = cur_node->right;

    assert(arg_1);
    assert(arg_2);

    if (arg_1->type == NUM && arg_1->val.num == 0)          // 0 / f(x) --> 0
    {
        cur_node->type    = NUM;
        cur_node->val.num = 0;
        cur_node->left    = NULL;
        cur_node->right   = NULL;

        RemoveNode    (tree, &arg_1);
        RemoveSubtree (tree, &arg_2); 

        *was_simplified = true;       
    }

    else if (arg_2->type == NUM && arg_2->val.num == 0)
    {
        fprintf(stderr, "Warning! division by zero\n");
        return cur_node;
    }

    else if (arg_2->type == NUM && arg_2->val.num == 1)     // f(x) / 1 --> f(x)
    {
        *cur_node = *arg_1;

        RemoveNode(tree, &arg_1);
        RemoveNode(tree, &arg_2);    

        *was_simplified = true;    
    }

    return cur_node;
}

Node *SimpleDegVars(Tree *tree, Node *cur_node, bool *was_simplified)
{
    Node *arg_1 = cur_node->left;
    Node *arg_2 = cur_node->right;

    assert(arg_1);
    assert(arg_2);

    if (arg_1->type == NUM && arg_1->val.num == 0)              // 0 ^ f(x)  -->  0
    {
        cur_node->type    = NUM;
        cur_node->val.num = 0;
        cur_node->left    = NULL;
        cur_node->right   = NULL;

        RemoveNode    (tree, &arg_1);
        RemoveSubtree (tree, &arg_2);    

        *was_simplified = true;    
    }

    else if ((arg_1->type == NUM && arg_1->val.num == 1) || (arg_2->type == NUM && arg_2->val.num == 0))         // 1 ^ f(x) || f(x) ^ 0  -->  1
    {
        cur_node->type    = NUM;
        cur_node->val.num = 1;

        RemoveNode(tree, &arg_1);
        RemoveNode(tree, &arg_2);     

        *was_simplified = true;   
    }

    else if (arg_2->type == NUM && arg_2->val.num == 1)     // f(x) ^ 1 --> f(x)
    {
        *cur_node = *arg_1;

        RemoveNode(tree, &arg_1);
        RemoveNode(tree, &arg_2);      

        *was_simplified = true;  
    }

    return cur_node;
}