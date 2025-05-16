#ifndef BACK_WRITER_H
#define BACK_WRITER_H

#include <stdio.h>

#include "../../tree/lib/tree_lib.h"

#define BASE_OUTPUT_ASM_FILE_NAME  "../res_asm.asm"

#define MAIN_FUNC_NAME  "MAIN_HOI"

#ifndef COMMENT_SYMBOL
#define COMMENT_SYMBOL  ';'
#endif

FILE *GetOutputAsmFile(const int argc, const char *argv[]);

void PrintAsmCodeByNode (Node *node,    FILE *dest_file);
void PrintMathOpAsm     (Node *math_op, FILE *dest_file);
void PopToEmptyRam      (FILE *asm_file);

//---------------------------------------------------------------------------------------------------------------//

struct MathOpAsmInfo
{
    MathOperation_enum num;
    const char *asm_symbol;
};

const MathOpAsmInfo MathOpAsmInfos[MATH_OPERATIONS_NUM] = 
{
    { TREE_BOOL_EQ,      "JNE"  },
    { TREE_BOOL_NEQ,     "JE"   },
    { TREE_BOOL_GREATER, "JB"   },
    { TREE_BOOL_LOWER,   "JA"   },
    { TREE_ADD,          "ADD"  },
    { TREE_SUB,          "SUB"  },
    { TREE_MUL,          "MUL"  },
    { TREE_DIV,          "DIV"  },
    { TREE_SQRT,         "SQRT" },
    { TREE_DEG,          NULL   },

    { TREE_LN,           NULL   },
    { TREE_LOG2,         NULL   },

    { TREE_SIN,          "SIN"  },
    { TREE_COS,          "COS"  },
    { TREE_TAN,          "TG"   }
};

//---------------------------------------------------------------------------------------------------------------//

void PrintStartRegisterValues(FILE *dest_file);

void PrintVarTAsm        (Node *var_t_node,    FILE *dest_file);
void PrintInitAsm        (Node *init_node,     FILE *dest_file);
void PrintAssignAsm      (Node *assign_node,   FILE *dest_file);
void PrintChildrenAsm    (Node *new_expr_node, FILE *dest_file);
void PrintIfAsm          (Node *if_node,       FILE *dest_file);
void PrintWhileAsm       (Node *while_node,    FILE *dest_file);
void PrintReturnAsm      (Node *ret_node,      FILE *dest_file);
void PrintCallAsm        (Node *call_node,     FILE *dest_file);
void PrintPassArgsInCall (Node *comma_node,    FILE *dest_file);
void PrintSpuInAsm       (Node *spu_in_node,   FILE *dest_file);
void PrintSpuOutAsm      (Node *spu_in_node,   FILE *dest_file);

struct KeyWordAsmInfo
{
    KeyWord_enum name;
    void (*PrintAsmCodeFunc)(Node *node, FILE *dest_file);
};

const KeyWordAsmInfo KeyWordAsmInfos[KEY_WORDS_NUM] = 
{
    { TREE_VAR_T_INDICATOR,  PrintVarTAsm     },
    { TREE_FUNC_T_INDICATOR, NULL             },
    { TREE_FUNC_CALL,        PrintCallAsm     },
    { TREE_INT_INIT,         PrintInitAsm     },
    { TREE_DOUBLE_INIT,      PrintInitAsm     },
    { TREE_NEW_EXPR,         PrintChildrenAsm },
    { TREE_COMMA,            PrintChildrenAsm },
    { TREE_NEW_FUNC,         PrintChildrenAsm },
    { TREE_ASSIGN,           PrintAssignAsm   },
    { TREE_IF,               PrintIfAsm       },
    { TREE_WHILE,            PrintWhileAsm    },
    { TREE_RETURN,           PrintReturnAsm   },
    { TREE_SPU_IN,           PrintSpuInAsm    },
    { TREE_SPU_OUT,          PrintSpuOutAsm   },
};

#endif