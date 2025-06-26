#ifndef ELF_INFO_H
#define ELF_INFO_H

#include <stdio.h>
#include <stdint.h>


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
    uint32_t  p_type;   // ��� �������� (PT_LOAD = 1)
    uint32_t  p_flags;  // ����� (PF_X, PF_W, PF_R)
    uint64_t  p_offset; // �������� � �����
    uint64_t  p_vaddr;  // ����������� ����� � ������
    uint64_t  p_paddr;  // ���������� ����� (������ = vaddr)
    uint64_t  p_filesz; // ������ � �����
    uint64_t  p_memsz;  // ������ � ������
    uint64_t  p_align;  // ������������
};

struct ByteCode
{
    char *data;
    size_t size;
};


struct ElfData
{
    ElfHeader     elf_hdr;
    ProgramHeader prog_hdr;
    ByteCode      code;
};

const uint64_t CODE_OFFSET = 0x78;


#include "elf_debug.h"

void ElfHeaderFill      (ElfHeader *elf_hdr);
void ProgramHeaderFill  (ProgramHeader *prog_hdr);

ElfFuncRes ElfDataCtor  (ElfData *elf_data);
ElfFuncRes ElfDataDtor  (ElfData *elf_data);
ElfFuncRes WriteCode    (FILE *elf_file, ElfData *elf_data);

#endif /*ELF_INFO_H*/