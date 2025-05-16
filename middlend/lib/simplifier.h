#ifndef SIMPLIFIER_H
#define SIMPLIFIER_H

#include <stdio.h>

#include "tree_lib.h"

void  SimplifyTree      (Tree *code_tree);

Node *SimplifyConstants (Tree *code_tree, Node *cur_node, bool *was_simplified);
Node *DelNeutralElements(Tree *code_tree, Node *cur_node, bool *was_simplified);

Node *CalculateNode     (Tree *tree,      Node *op_node);

Node *SimpleBoolEqVars  (Tree *tree,      Node *cur_node, bool *was_simplified);
Node *SimpleAddVars     (Tree *tree, Node *cur_node, bool *was_simplified);
Node *SimpleSubVars     (Tree *tree, Node *cur_node, bool *was_simplified);
Node *SimpleMulVars     (Tree *tree, Node *cur_node, bool *was_simplified);
Node *SimpleDivVars     (Tree *tree, Node *cur_node, bool *was_simplified);
Node *SimpleDegVars     (Tree *tree, Node *cur_node, bool *was_simplified);

struct MathOpSimplifierInfo
{
    MathOperation_enum num;
    Node* (*simpl_vars_func) (Tree *tree, Node *cur_node, bool *was_simplified);
};

const MathOpSimplifierInfo MathOpSimplifierInfos[MATH_OPERATIONS_NUM] =
{
    { TREE_BOOL_EQ,      SimpleBoolEqVars   },
    { TREE_BOOL_NEQ,     DelNeutralElements },
    { TREE_BOOL_LOWER,   DelNeutralElements },
    { TREE_BOOL_GREATER, DelNeutralElements },
    { TREE_ADD,          SimpleAddVars      },
    { TREE_SUB,          SimpleSubVars      },
    { TREE_MUL,          SimpleMulVars      },
    { TREE_DIV,          SimpleDivVars      },
    { TREE_DEG,          SimpleDegVars      },

    { TREE_LN,           DelNeutralElements },
    { TREE_LOG2,         DelNeutralElements },

    { TREE_SIN,          DelNeutralElements },
    { TREE_COS,          DelNeutralElements },
};

#endif