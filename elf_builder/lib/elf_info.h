#ifndef ELF_INFO_H
#define ELF_INFO_H

#include <stdio.h>
#include <stdint.h>

#include "ir.h"

const size_t MAX_CODE_SIZE = 4096;


struct ElfHeader
{
    unsigned char e_ident[16];  // ����� (0x7F 'E' 'L' 'F'), �������� (32/64), endianness, ������ ELF
    uint16_t      e_type;       // ��� ����� (ET_EXEC, ET_DYN, ET_REL)
    uint16_t      e_machine;    // ����������� (EM_X86_64 = 0x3E)
    uint32_t      e_version;    // ������ ELF (������ EV_CURRENT = 1)
    uint64_t      e_entry;      // ����� ����� (�����, ��� ���������� ����������)
    uint64_t      e_phoff;      // �������� �� Program Headers
    uint64_t      e_shoff;      // �������� �� Section Headers (����� ���� 0)
    uint32_t      e_flags;      // ����� (��� x86-64 ������ 0)
    uint16_t      e_ehsize;     // ������ ELF-��������� (64 �����)
    uint16_t      e_phentsize;  // ������ ������ Program Header
    uint16_t      e_phnum;      // ���������� Program Headers
    uint16_t      e_shentsize;  // ������ ������ Section Header
    uint16_t      e_shnum;      // ���������� Section Headers
    uint16_t      e_shstrndx;   // ������ ��������� ������� ������ (����� ���� 0)
};

struct ProgramHeader
{
    uint32_t  p_type;           // ��� �������� (PT_LOAD = 1)
    uint32_t  p_flags;          // ����� (PF_X, PF_W, PF_R)
    uint64_t  p_offset;         // �������� � �����
    uint64_t  p_vaddr;          // ����������� ����� � ������
    uint64_t  p_paddr;          // ���������� ����� (������ = vaddr)
    uint64_t  p_filesz;         // ������ � �����
    uint64_t  p_memsz;          // ������ � ������
    uint64_t  p_align;          // ������������
};

struct SectionHeader
{
    uint32_t sh_name;           // �������� � .shstrtab �� ����� ������    
    uint32_t sh_type;           // ��� ������ (SHT_PROGBITS, SHT_STRTAB, ...)
    uint64_t sh_flags;          // ����� �������   
    uint64_t sh_addr;           // ����� � ������
    uint64_t sh_offset;         // �������� ������ � �����    
    uint64_t sh_size;           // ������ ������
    uint32_t sh_link;           // ������� �� ���� (��������, .symtab ��������� �� .strtab)    
    uint32_t sh_info;           // ��� ����������, ������� �� ����
    uint64_t sh_addralign;      // ��������� ������������    
    uint64_t sh_entsize;        // ������ ��������, ���� ��� �������
};


const size_t LABEL_LEN = 40;
const size_t MAX_LABEL_NUM = 200;
const size_t MAX_FIXUP_NUM = 200;

const size_t POISON_LABEL_INDX = 0xBABACED;

struct TextLabels
{
    size_t label_nums[MAX_LABEL_NUM];   // ����� �����
    size_t label_vals[MAX_LABEL_NUM];   // ���� ���������

    size_t size;
};

struct TextFixups
{
    size_t label_addrs[MAX_FIXUP_NUM];  // ��� ����������� ��������� � �����
    size_t label_nums [MAX_FIXUP_NUM];  // ����� ���� �����

    size_t size;
};


struct TextSection
{
    char code[MAX_CODE_SIZE];
    size_t size;

    TextLabels labels;
    TextFixups fixups;
};


struct ElfData
{
    ElfHeader     elf_hdr;
    ProgramHeader prog_hdr;
    TextSection   text_sect;
    const char   *shstrtab;

    // ������������ � ���������� ������ (���������� �����)
    size_t sect_hdrs_allign;
    
    SectionHeader null_header;
    SectionHeader text_sect_hdr;
    SectionHeader shstrtab_hdr;
};

const uint64_t ENTRY_ADDR  = 0x400000;
const uint64_t CODE_OFFSET = 0x80;
const uint64_t SECT_HDR_ALLIGN = 0x10;

const char SHSTRTAB_ELF[17] = "\0.text\0.shstrtab";

#include "elf_debug.h"

void ElfHeaderFill             (ElfData *elf_data);
void ProgramHeaderFill         (ElfData *elf_data);
void TextSectionHeaderFill     (ElfData *elf_data);
void ShstrtabSectionHeaderFill (ElfData *elf_data);

ElfFuncRes ElfDataFill  (ElfData *elf_data, IR_struct *ir);
ElfFuncRes WriteCode    (FILE *elf_file, ElfData *elf_data);

#endif /*ELF_INFO_H*/