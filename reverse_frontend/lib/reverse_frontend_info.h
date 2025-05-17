#ifndef REVERSE_FRONTEND_INFO_H
#define REVERSE_FRONTEND_INFO_H

#include <stdio.h>

#include "tree_lib.h"

#define REFRONT_FILE_NAME  "../../refront.txt"

void PrintNodeMySym       (Node *node, FILE *dest_file);

void ReverseFrontVarT     (Node *var_t_node,    FILE *dest_file);
void ReverseFrontFuncT    (Node *func_t_node,   FILE *dest_file);
void ReverseFrontInit     (Node *init_node,     FILE *dest_file);
void ReverseFrontNewExpr  (Node *new_expr_node, FILE *dest_file);
void ReverseFrontendComma (Node *cur_comma,     FILE *dest_file);
void ReverseFrontAssign   (Node *assign_node,   FILE *dest_file);
void ReverseFrontIfWhile  (Node *cur_node,      FILE *dest_file);
void ReverseFrontReturn   (Node *cur_node,      FILE *dest_file);
void ReverseFrontSpuIn    (Node *cur_node,      FILE *dest_file);
void ReverseFrontSpuOut   (Node *cur_node,      FILE *dest_file);
void ReverseFrontChildren (Node *node,          FILE *dest_file);


struct KeyWordReverseFrontendInfo
{
    KeyWord_enum num;
    void (*reverse_frontend_func) (Node *cur_node, FILE *dest_file);
};

const KeyWordReverseFrontendInfo KeyWordReverseFrontendInfos[KEY_WORDS_NUM] = 
{
    { TREE_VAR_T_INDICATOR,  ReverseFrontVarT     },
    { TREE_FUNC_T_INDICATOR, ReverseFrontFuncT    },
    { TREE_FUNC_CALL,        ReverseFrontChildren },
    { TREE_INIT,             ReverseFrontInit     },
    { TREE_NEW_EXPR,         ReverseFrontNewExpr  },
    { TREE_COMMA,            ReverseFrontendComma },
    { TREE_NEW_FUNC,         ReverseFrontChildren },
    { TREE_ASSIGN,           ReverseFrontAssign   },
    { TREE_IF,               ReverseFrontIfWhile  },
    { TREE_WHILE,            ReverseFrontIfWhile  },
    { TREE_RETURN,           ReverseFrontReturn   },
    { TREE_SPU_IN,           ReverseFrontSpuIn    },
    { TREE_SPU_OUT,          ReverseFrontSpuOut   },
};

#endif
