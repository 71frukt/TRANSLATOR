#include <stdio.h>
#include <string.h>

#include "logger.h"
#include "elf_info.h"
#include "instructions_to_bytes.h"

void PushReg(TextSection *text, RegCode reg_code)
{
    lassert(text);

    text->code[text->size++] = (char) 0x50 + (char) reg_code;
}

void PushImm(TextSection *text, int imm_32)
{
    lassert(text);

    text->code[text->size++] = (char) 0x68;

    memcpy(text->code + text->size, &imm_32, 4);
    text->size += 4;
}

void PushVar(TextSection *text, size_t var) // RegCode base_reg, int shift_imm_32)
{
    lassert(text);

    text->code[text->size++] = (char) 0x48;    // REX
    text->code[text->size++] = (char) 0xFF;    // opcode

    text->code[text->size++] = (char) 0xB0 + RBP_CODE;

    int shift_imm_32 = -8 * (int) var;
    memcpy(text->code + text->size, &shift_imm_32, 4);
    text->size += 4;
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

void MovRegImm(TextSection *text, RegCode reg_code, long imm_64)
{
    lassert(text);

    text->code[text->size++] = (char) 0x48;                 // REX
    text->code[text->size++] = (char) (0xB8 + reg_code);    // opcode

    memcpy(text->code + text->size, &imm_64, 8);
    text->size += 8;
}


void MovRegVar(TextSection *text, RegCode dest_reg, size_t var) // mov reg, [base_reg + shift_imm_32]
{
    lassert(text);

    text->code[text->size++] = (char) 0x48;    // REX
    text->code[text->size++] = (char) 0x8B;    // opcode

    text->code[text->size++] = (char) (RBP_CODE | dest_reg << 3 | 0b10 << 6);

    int shift_imm_32 = -8 * (int) var;
    memcpy(text->code + text->size, &shift_imm_32, 4);
    text->size += 4;
}

void MovVarReg(TextSection *text, int var, RegCode source_reg)  // mov [base_reg + shift_imm_32], reg
{
    lassert(text);

    text->code[text->size++] = (char) 0x48;    // REX
    text->code[text->size++] = (char) 0x89;    // opcode

    text->code[text->size++] = (char) (RBP_CODE | source_reg << 3 | 0b10 << 6);

    int shift_imm_32 = -8 * (int) var;
    memcpy(text->code + text->size, &shift_imm_32, 4);
    text->size += 4;
}

void MovVarImm(TextSection *text, size_t var, int source_imm_32)  // mov [base_reg + shift_mm_31], source_imm_32
{
    lassert(text);

    text->code[text->size++] = (char) 0x48;    // REX
    text->code[text->size++] = (char) 0xC7;    // opcode

    text->code[text->size++] = (char) (RBP_CODE | 0b10 << 6);

    int shift_imm_32 = -8 * (int) var;
    memcpy(text->code + text->size, &shift_imm_32, 4);
    text->size += 4;

    memcpy(text->code + text->size, &source_imm_32, 4);
    text->size += 4;
}


void CMovRegReg(TextSection *text, RegCode reg1_code, RegCode reg2_code, CMovCode cmov_code)
{
    lassert(text);

    text->code[text->size++] = (char) 0x48;    // REX
    text->code[text->size++] = (char) 0x0F;    // opcode
    text->code[text->size++] = (char) cmov_code;
    text->code[text->size++] = (char) (0b11000000 | reg2_code | reg1_code << 3);
}


void AddRegReg(TextSection *text, RegCode reg1, RegCode reg2)
{
    lassert(text);

    text->code[text->size++] = (char) 0x48;
    text->code[text->size++] = (char) 0x01;

    text->code[text->size++] = (char) (reg1 | reg2 << 3 | 0b11 << 6);
}

void AddRegImm(TextSection *text, RegCode reg, int imm_32)
{
    lassert(text);

    text->code[text->size++] = (char) 0x48;
    text->code[text->size++] = (char) 0x81;

    text->code[text->size++] = (char) (reg | 0b11 << 6);

    memcpy(text->code + text->size, &imm_32, 4);
    text->size += 4;
}

void SubRegReg(TextSection *text, RegCode reg1, RegCode reg2)
{
    lassert(text);

    text->code[text->size++] = (char) 0x48;
    text->code[text->size++] = (char) 0x29;

    text->code[text->size++] = (char) (reg1 | reg2 << 3 | 0b11 << 6);
}

void SubRegImm(TextSection *text, RegCode reg, int imm_32)
{
    lassert(text);

    text->code[text->size++] = (char) 0x48;
    text->code[text->size++] = (char) 0x81;

    text->code[text->size++] = (char) (reg | 0b101 << 3 | 0b11 << 6);

    memcpy(text->code + text->size, &imm_32, 4);
    text->size += 4;
}

void MulReg(TextSection *text, RegCode reg)
{
    lassert(text);

    text->code[text->size++] = (char) 0x48;
    text->code[text->size++] = (char) 0xF7;

    text->code[text->size++] = (char) (reg | 0b100 << 3 | 0b11 << 6);
}

void IDivReg(TextSection *text, RegCode reg)
{
    lassert(text);

    text->code[text->size++] = (char) 0x48;
    text->code[text->size++] = (char) 0xF7;

    text->code[text->size++] = (char) (reg | 0b111 << 3 | 0b11 << 6);
}

void XorRegReg(TextSection *text, RegCode reg1, RegCode reg2)
{
    lassert(text);

    text->code[text->size++] = (char) 0x48;
    text->code[text->size++] = (char) 0x31;

    text->code[text->size++] = (char) (reg1 | reg2 << 3 | 0b11 << 6);
}

void ConvQuadwToOctow(TextSection *text)
{
    lassert(text);

    text->code[text->size++] = (char) 0x48;
    text->code[text->size++] = (char) 0x99;
}

void CmpRegReg(TextSection *text, RegCode reg1, RegCode reg2)
{
    lassert(text);

    text->code[text->size++] = (char) 0x48;    // REX
    text->code[text->size++] = (char) 0x39;    // opcode
    text->code[text->size++] = (char) (0b11000000 | reg1 | reg2 << 3);
}

void FictitiousJump(TextSection *text, CondType cond_type, size_t label_num)      // заполняет поле смещения нулями и делает запись в фиксапы
{
    switch (cond_type)
    {
    case DO_IF_TRUE:
        text->code[text->size++] = (char) 0x0F;
        text->code[text->size++] = (char) 0x85;     // jnz
        text->size += 4;
        break;
    
    case DO_IF_FALSE:
        text->code[text->size++] = (char) 0x0F;
        text->code[text->size++] = (char) 0x84;     // jz
        break;
        
    case DO_ANYWAY:
        text->code[text->size++] = (char) 0xE9;     // jmp
        break;

        default:
        log(ERROR, "Incorrect cond_type!");
        break;
    }

    size_t fixups_size = text->fixups.size++;
    text->fixups.label_addrs[fixups_size] = text->size;
    text->fixups.label_nums [fixups_size] = label_num;
    
    text->size += 4;    // временные нули
}

void FictitiousCall(TextSection *text, size_t func_num)     // номера функций и локальных меток НЕ ПЕРЕСЕКАЮТСЯ, так устроен IR
{
    text->code[text->size++] = (char) 0xE8;     // call

    size_t fixups_size = text->fixups.size++;
    text->fixups.label_addrs[fixups_size] = text->size;
    text->fixups.label_nums [fixups_size] = func_num;
    
    text->size += 4;    // временные нули
}

void FuncRet(TextSection *text)
{
    text->code[text->size++] = (char) 0xC3;     // ret
}

void TestRaxRax(TextSection *text)
{
    text->code[text->size++] = (char) 0x48;
    text->code[text->size++] = (char) 0x85;
    text->code[text->size++] = (char) 0xC0;
}