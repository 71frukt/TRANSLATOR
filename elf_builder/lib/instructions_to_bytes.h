#ifndef INSTRUCTIONS_TO_BYTES_H
#define INSTRUCTIONS_TO_BYTES_H

#include <stdio.h>

enum RegCode
{
    RAX_CODE = 0b000,
    RCX_CODE = 0b001,
    RDX_CODE = 0b010,
    RBX_CODE = 0b011,
    RSP_CODE = 0b100,
    RBP_CODE = 0b101,
    RSI_CODE = 0b110,
    RDI_CODE = 0b111
};

const size_t PUSH_SIZE = 1;
const size_t MOV_SIZE  = 3;

const size_t IMM_SIZE  = 8;

void PushReg   (TextSection *text, RegCode reg_code);
void PopReg    (TextSection *text, RegCode reg_code);
void MovRegReg (TextSection *text, RegCode reg1_code, RegCode reg2_code);
void MovRegImm (TextSection *text, RegCode reg_code, long imm);
void MovRegMem (TextSection *text, RegCode dest_reg , RegCode base_reg, int scale_imm); // mov reg, [base_reg + shift_reg * scale_imm]

// #define PUSH_REG_CODE(_reg_code)  (0x50 + _reg_code)
// #define POP_REG_CODE(_reg_num)   (0x58 + _reg_num)

// #define MOV_REG_REG(_reg1_num, _reg2_num)  {0x48, 0x89, 0b11000000 | _reg1_num | _reg2_num << 3}
// #define MOV_REG_IMM(_reg_code, _imm)       {0x48, 0xB8 + _reg_code, _imm}

#endif /*INSTRUCTIONS_TO_BYTES_H*/