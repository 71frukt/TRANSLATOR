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

enum CondType
{
    DO_IF_TRUE,
    DO_IF_FALSE,
    DO_ANYWAY
};

enum CMovCode
{
    CMOV_E  = 0x44,
    CMOV_NE = 0x45,
    CMOV_L  = 0x4C,
    CMOV_LE = 0x4E,
    CMOV_G  = 0x4F,
    CMOV_GE = 0x4D
};

const size_t PUSH_SIZE = 1;
const size_t MOV_SIZE  = 3;

const size_t IMM_SIZE  = 8;

void PushReg   (TextSection *text, RegCode reg_code);
void PushVar   (TextSection *text, size_t var);
void PushImm   (TextSection *text, int imm_32);

void PopReg    (TextSection *text, RegCode reg_code);

void MovRegReg (TextSection *text, RegCode reg1_code, RegCode reg2_code);
void CMovRegReg(TextSection *text, RegCode reg1_code, RegCode reg2_code, CMovCode cmov_code);
void MovRegImm (TextSection *text, RegCode reg_code , long imm);
void MovRegVar (TextSection *text, RegCode dest_reg , size_t var);          // mov reg, [base_reg + shift_imm_32]
void MovVarReg(TextSection *text , int     var      , RegCode source_reg);  // mov [base_reg + shift_imm_32], reg
void MovVarImm (TextSection *text, size_t var       , int source_imm_32);   // mov [base_reg + shift_mm_31], source_imm_32

void AddRegReg (TextSection *text, RegCode reg1, RegCode reg2);
void SubRegReg (TextSection *text, RegCode reg1, RegCode reg2);
void AddRegImm (TextSection *text, RegCode reg , int     imm_32);
void SubRegImm (TextSection *text, RegCode reg , int     imm_32);
void XorRegReg (TextSection *text, RegCode reg1, RegCode reg2);
void MulReg    (TextSection *text, RegCode reg);
void IDivReg   (TextSection *text, RegCode reg);

void CmpRegReg (TextSection *text, RegCode reg1, RegCode reg2);

void ConvQuadwToOctow(TextSection *text);


void FictitiousJump (TextSection *text, CondType cond_type, size_t label_num);  // заполняет поле смещения нулями и делает запись в фиксапы
void FictitiousCall (TextSection *text, size_t func_num);                       // номера функций и локальных меток НЕ ПЕРЕСЕКАЮТСЯ, так устроен IR

void FuncRet (TextSection *text);

void TestRaxRax(TextSection *text);

#endif /*INSTRUCTIONS_TO_BYTES_H*/