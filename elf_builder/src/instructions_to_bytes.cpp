#include <stdio.h>
#include <string.h>

#include "logger.h"
#include "elf_info.h"
#include "instructions_to_bytes.h"

void PushReg(TextSection *text, RegCode reg_code)
{
    lassert(text);

    text->code[text->size] = (char) 0x50 + (char) reg_code;
    text->size++;
}

void PopReg(TextSection *text, RegCode reg_code)
{
    lassert(text);

    text->code[text->size] = (char) 0x58 + (char) reg_code;
    text->size++;
}

void MovRegReg(TextSection *text, RegCode reg1_code, RegCode reg2_code)
{
    lassert(text);

    text->code[text->size++] = (char) 0x48;    // REX
    text->code[text->size++] = (char) 0x89;    // opcode
    text->code[text->size++] = (char) (0b11000000 | reg1_code | reg2_code << 3);
}

void MovRegImm(TextSection *text, RegCode reg_code, long imm)
{
    // {0x48, 0xB8 + _reg_code, _imm}

    lassert(text);

    text->code[text->size++] = (char) 0x48;                 // REX
    text->code[text->size++] = (char) (0xB8 + reg_code);    // opcode

    memcpy(text->code + text->size, &imm, IMM_SIZE);
    text->size += IMM_SIZE;
}


void MovRegMem(TextSection *text, RegCode dest_reg, RegCode base_reg, int shift_imm)   // mov reg, [base_reg + imm]
{
    lassert(text);

    text->code[text->size++] = (char) 0x48;    // REX
    text->code[text->size++] = (char) 0x8B;    // opcode

    text->code[text->size++] = (char) (base_reg | dest_reg << 3 | 0b10 << 6);

    memcpy(text->code + text->size, &shift_imm, 4);
    text->size += 4;
}

// void MovRegMem(TextSection *text, RegCode dest_reg, RegCode base_reg, RegCode shift_reg, char scale_imm)   // mov reg, [base_reg + shift_reg * scale_imm]
// {
//     lassert(text);

//     text->code[text->size++] = 0x48;    // REX
//     text->code[text->size++] = 0x8B;    // opcode

//     text->code[text->size++] = base_reg << 3 | 0x100;
//     text->code[text->size++] = base_reg | (shift_reg << 3) | (scale_imm << 6);
// }