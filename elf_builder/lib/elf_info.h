#ifndef ELF_INFO_H
#define ELF_INFO_H

#include <stdio.h>
#include <stdint.h>


struct ElfHeader
{
    unsigned char e_ident[16];  // Магия (0x7F 'E' 'L' 'F'), битность (32/64), endianness, версия ELF
    uint16_t      e_type;       // Тип файла (ET_EXEC, ET_DYN, ET_REL)
    uint16_t      e_machine;    // Архитектура (EM_X86_64 = 0x3E)
    uint32_t      e_version;    // Версия ELF (обычно EV_CURRENT = 1)
    uint64_t      e_entry;      // Точка входа (адрес, где начинается выполнение)
    uint64_t      e_phoff;      // Смещение до Program Headers
    uint64_t      e_shoff;      // Смещение до Section Headers (может быть 0)
    uint32_t      e_flags;      // Флаги (для x86-64 обычно 0)
    uint16_t      e_ehsize;     // Размер ELF-заголовка (64 байта)
    uint16_t      e_phentsize;  // Размер одного Program Header
    uint16_t      e_phnum;      // Количество Program Headers
    uint16_t      e_shentsize;  // Размер одного Section Header
    uint16_t      e_shnum;      // Количество Section Headers
    uint16_t      e_shstrndx;   // Индекс строковой таблицы секций (может быть 0)
};

struct ProgramHeader
{
    uint32_t  p_type;   // Тип сегмента (PT_LOAD = 1)
    uint32_t  p_flags;  // Флаги (PF_X, PF_W, PF_R)
    uint64_t  p_offset; // Смещение в файле
    uint64_t  p_vaddr;  // Виртуальный адрес в памяти
    uint64_t  p_paddr;  // Физический адрес (обычно = vaddr)
    uint64_t  p_filesz; // Размер в файле
    uint64_t  p_memsz;  // Размер в памяти
    uint64_t  p_align;  // Выравнивание
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