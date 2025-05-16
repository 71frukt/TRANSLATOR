#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <stdio.h>

#include "tree_elem_t_info.h"

enum FuncEntryForm
{
    PREFIX,
    INFIX
};

enum FuncType
{
    UNARY,
    BINARY
};

enum MathOperation_enum
{
    TREE_BOOL_EQ,
    TREE_BOOL_NEQ,
    TREE_BOOL_LOWER,
    TREE_BOOL_GREATER,
    TREE_BOOL_LOWER_EQ,
    TREE_BOOL_GREATER_EQ,
    TREE_ADD,
    TREE_SUB,
    TREE_MUL,
    TREE_DIV,
    TREE_SQRT,
    TREE_DEG,

    TREE_LN,
    TREE_LOG2,

    TREE_SIN,
    TREE_COS,
    TREE_TAN
};

struct Node;
struct Tree;

TreeElem_t IsEqual    (Node *arg1, Node *arg2);
TreeElem_t IsNotEqual (Node *arg1, Node *arg2);
TreeElem_t IsBelow    (Node *arg1, Node *arg2);
TreeElem_t IsAbove    (Node *arg1, Node *arg2);
TreeElem_t IsBelowEq  (Node *arg1, Node *arg2);
TreeElem_t IsAboveEq  (Node *arg1, Node *arg2);


TreeElem_t Add        (Node *arg1, Node *arg2);
TreeElem_t Sub        (Node *arg1, Node *arg2);
TreeElem_t Mul        (Node *arg1, Node *arg2);
TreeElem_t Div        (Node *arg1, Node *arg2);

TreeElem_t Sqrt       (Node *arg1, Node *arg2);
TreeElem_t Deg        (Node *arg1, Node *arg2);
TreeElem_t Ln         (Node *arg1, Node *arg2);
TreeElem_t Log        (Node *arg1, Node *arg2);

TreeElem_t Sin        (Node *arg1, Node *arg2);
TreeElem_t Cos        (Node *arg1, Node *arg2);
TreeElem_t Tan        (Node *arg1, Node *arg2);

struct MathOperation
{
    const MathOperation_enum  num;
    const char           *my_symbol;
    const char           *real_symbol;
    const char           *asm_symbol;

    const FuncType       type;                                               // UNARY  / BINARY
    const FuncEntryForm  form;                                               // PREFIX / INFIX

    TreeElem_t  (*op_func)  (Node *arg1, Node *arg2);
};

const int MATH_OPERATIONS_NUM = 17;

// const MathOperation MathOperations[MATH_OPERATIONS_NUM] = // FIXME когда буду переделывать обратно не забыть JAE JBE
// {    
//     { BOOL_EQ,      "будто",      "==",  "JNE",  BINARY, INFIX,  IsEqual    },
//     { BOOL_NEQ,     "ну_нафиг",   "!=",  "JE",   BINARY, INFIX,  IsNotEqual },
//     { BOOL_LOWER,   "хиленький",  "\\<", "JA",   BINARY, INFIX,  IsBelow    },         // JA для JB специально 
//     { BOOL_GREATER, "мощный",     "\\>", "JB",   BINARY, INFIX,  IsAbove    },         // JB для JA специально   
//     { ADD,          "накину",     "+",   "ADD",  BINARY, INFIX,  Add        },
//     { SUB,          "сдеру",      "-",   "SUB",  BINARY, INFIX,  Sub        },
//     { MUL,          "помножу",    "*",   "MUL",  BINARY, INFIX,  Mul        },
//     { DIV,          "порублю",    "/",   "DIV",  BINARY, INFIX,  Div        },
//     { SQRT,         "накорню",    "cor", "SQRT", UNARY,  PREFIX, Sqrt       },
//     { DEG,          "вздыбить",   "^",   NULL,   BINARY, INFIX,  Deg        },
     
//     { LN,           "натурально", "ln",  NULL,   UNARY,  PREFIX, Ln        },
//     { LOG2,          "лог",        "log", NULL,   BINARY, PREFIX, Log       },
     
//     { SIN,          "син",        "sin", "SIN",  UNARY,  PREFIX, Sin       },
//     { COS,          "кос",        "cos", "COS",  UNARY,  PREFIX, Cos       },
//     { TAN,          "тан",        "tg",  "TG",   UNARY,  PREFIX, Tan       }
// };

const MathOperation MathOperations[MATH_OPERATIONS_NUM] = 
{
    { TREE_BOOL_EQ,         "будто",    "==",   "JNE",  BINARY, INFIX,  IsEqual    },
    { TREE_BOOL_NEQ,        "!=",       "!=",   "JE",   BINARY, INFIX,  IsNotEqual },
    { TREE_BOOL_LOWER,      "<",        "\\<",  "JA",   BINARY, INFIX,  IsBelow    },         // JA для JB специально 
    { TREE_BOOL_GREATER,    ">",        "\\>",  "JB",   BINARY, INFIX,  IsAbove    },         // JB для JA специально   
    { TREE_BOOL_LOWER_EQ,   "<=",       "\\<=", "JAE",  BINARY, INFIX,  IsBelowEq  },         // JAE для JBE специально 
    { TREE_BOOL_GREATER_EQ, ">=",       "\\>=", "JBE",  BINARY, INFIX,  IsAboveEq  },         // JBE для JAE специально   
    { TREE_ADD,             "+",        "+",    "ADD",  BINARY, INFIX,  Add        },
    { TREE_SUB,             "-",        "-",    "SUB",  BINARY, INFIX,  Sub        },
    { TREE_MUL,             "*",        "*",    "MUL",  BINARY, INFIX,  Mul        },
    { TREE_DIV,             "/",        "/",    "DIV",  BINARY, INFIX,  Div        },
    { TREE_SQRT,            "корень",   "cor",  "SQRT", UNARY,  PREFIX, Sqrt       },
    { TREE_DEG,             "^",        "^",    NULL,   BINARY, INFIX,  Deg        },

    { TREE_LN,              "лн",       "ln",   NULL,   UNARY,  PREFIX, Ln         },
    { TREE_LOG2,            "лог",      "log",  NULL,   BINARY, PREFIX, Log        },

    { TREE_SIN,             "син",      "sin",  "SIN",  UNARY,  PREFIX, Sin        },
    { TREE_COS,             "кос",      "cos",  "COS",  UNARY,  PREFIX, Cos        },
    { TREE_TAN,             "тан",      "tg",   "TG",   UNARY,  PREFIX, Tan        }
};

//------------------------------------------------------------------------------------------------------------//

enum Managers_enum
{
    OPEN_BLOCK_BRACKET,
    CLOSE_BLOCK_BRACKET,
    OPEN_EXPR_BRACKET,
    CLOSE_EXPR_BRACKET,
    EOT
};

struct ManageElem
{
    const Managers_enum  name;
    const char          *my_symbol;
    const char          *real_symbol;
};

const int MANAGE_ELEMS_NUM = 5;

// const ManageElem Managers[MANAGE_ELEMS_NUM] = 
// {
//     { OPEN_BLOCK_BRACKET,  "панки_хой",    "{"   },
//     { CLOSE_BLOCK_BRACKET, "попса_отстой", "}"   },
//     { OPEN_EXPR_BRACKET,   "желаешь",      "("   },
//     { CLOSE_EXPR_BRACKET,  "расскажу",     ")"   },
//     { EOT,                 "$",            "EOT" }
// };


const ManageElem Managers[MANAGE_ELEMS_NUM] = 
{
    { OPEN_BLOCK_BRACKET,  "{", "{"   },
    { CLOSE_BLOCK_BRACKET, "}", "}"   },
    { OPEN_EXPR_BRACKET,   "(", "("   },
    { CLOSE_EXPR_BRACKET,  ")", ")"   },
    { EOT,                 "$", "EOT" }
};

//------------------------------------------------------------------------------------------------------------//


enum KeyWord_enum
{
    TREE_VAR_T_INDICATOR,
    TREE_FUNC_T_INDICATOR,
    TREE_FUNC_CALL,
    TREE_INT_INIT,
    TREE_DOUBLE_INIT,
    TREE_NEW_EXPR,
    TREE_COMMA,
    TREE_NEW_FUNC,
    TREE_ASSIGN,
    TREE_IF,
    TREE_WHILE,
    TREE_RETURN,
    TREE_SPU_IN,
    TREE_SPU_OUT,
};

struct KeyWord
{
    const KeyWord_enum  name;
    const char         *my_symbol;
    const char         *real_symbol;
};

const int KEY_WORDS_NUM = 14;

// const KeyWord KeyWords[KEY_WORDS_NUM] = 
// {
//     { TREE_VAR_T_INDICATOR,  ""        , "var_t"     },
//     { TREE_FUNC_T_INDICATOR, ""        , "func_t"    },
//     { TREE_FUNC_CALL,        ""        , "call"      },
//     { TREE_INT_INIT,         "целый"   , "int"       },
//     { TREE_DOUBLE_INIT,      "дабл"    , "double"    },
//     { TREE_NEW_EXPR,         "хой"     , "new_line"  },
//     { TREE_COMMA,            "вместе"  , "param"     },
//     { TREE_NEW_FUNC,         ""        , "new_func"  },
//     { TREE_ASSIGN,           "теперь"  , "="         },
//     { TREE_IF,               "коль"    , "if"        },
//     { TREE_WHILE,            "пока"    , "while"     },
//     { TREE_RETURN,           "лес"     , "return"    },
//     { TREE_SPU_IN,           "почитаю" , "scanf"     },
//     { TREE_SPU_OUT,          "болтал"  , "printf"    },
// };

const KeyWord KeyWords[KEY_WORDS_NUM] = 
{
    { TREE_VAR_T_INDICATOR,  "",      "var_t"     },
    { TREE_FUNC_T_INDICATOR, "",      "func_t"    },
    { TREE_FUNC_CALL,        "",      "call"      },
    { TREE_INT_INIT,         "инт",   "int"       },
    { TREE_DOUBLE_INIT,      "дабл",  "double"    },
    { TREE_NEW_EXPR,         ";",     "new_line"  },
    { TREE_COMMA,            ",",     "param"     },
    { TREE_NEW_FUNC,         "",      "new_func"  },
    { TREE_ASSIGN,           "=" ,    "="         },
    { TREE_IF,               "если",  "if"        },
    { TREE_WHILE,            "пока",  "while"     },
    { TREE_RETURN,           "рет",   "return"    },
    { TREE_SPU_IN,           "скан",  "scanf"     },
    { TREE_SPU_OUT,          "принт", "printf"    },
};


//---------------------------------------------------------------------------------------------------------------//

enum SymbolMode
{
    MY_CODE_MODE,
    REAL_CODE_MODE
};

const MathOperation *GetOperationBySymbol  (char *sym, SymbolMode mode);
const KeyWord       *GetKeyWordBySymbol    (char *sym, SymbolMode mode);
const ManageElem    *GetManageElemBySymbol (char *sym, SymbolMode mode);


#endif