#ifndef ASM_OPERATIONS_H
#define ASM_OPERATIONS_H

enum AsmOperations_enum
{
    PUSH_ASM,
    POP_ASM,
    CALL_ASM,
    JMP_ASM,
    JA_ASM,
    JAE_ASM,
    JB_ASM,
    JBE_ASM,
    JE_ASM,
    JNE_ASM,
    RET_ASM,
    ADD_ASM,
    SUB_ASM,
    MUL_ASM,
    DIV_ASM,
    SQRT_ASM,
    MOD_ASM,
    SIN_ASM,
    COS_ASM,
    TG_ASM,
    CTG_ASM,
    CRTWND_ASM,
    DRAW_ASM,
    SETRNDRAM_ASM,
    DUMP_ASM,
    SPU_IN_ASM,
    SPU_OUT_ASM,
    HLT_ASM
};

struct AsmOperation
{
    AsmOperations_enum num;
    const char *sym;
};

const int ASM_MATH_OPERATIONS_NUM = 28;

const AsmOperation AsmOperations[ASM_MATH_OPERATIONS_NUM]
{
    { PUSH_ASM,         "PUSH"      },
    { POP_ASM,          "POP"       },
    { CALL_ASM,         "CALL"      },
    { JMP_ASM,          "JMP"       },
    { JA_ASM,           "JA"        },
    { JAE_ASM,          "JAE"       },
    { JB_ASM,           "JB"        },
    { JBE_ASM,          "JBE"       },
    { JE_ASM,           "JE"        },
    { JNE_ASM,          "JNE"       },
    { RET_ASM,          "RET"       },
    { ADD_ASM,          "ADD"       },
    { SUB_ASM,          "SUB"       },
    { MUL_ASM,          "MUL"       },
    { DIV_ASM,          "DIV"       },
    { SQRT_ASM,         "SQRT"      },
    { MOD_ASM,          "MOD"       },
    { SIN_ASM,          "SIN"       },
    { COS_ASM,          "COS"       },
    { TG_ASM,           "TG"        },
    { CTG_ASM,          "CTG"       },
    { CRTWND_ASM,       "CRTWND"    },
    { DRAW_ASM,         "DRAW"      },
    { SETRNDRAM_ASM,    "SETRNDRAM" },
    { DUMP_ASM,         "DUMP"      },
    { SPU_IN_ASM,       "SPU_IN"    },
    { SPU_OUT_ASM,      "TREE_SPU_OUT"   },
    { HLT_ASM,          "HLT"       }
};

#endif