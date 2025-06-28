#ifndef ELF_INFO_H
#define ELF_INFO_H

#include <stdio.h>
#include <stdint.h>

#include "ir.h"

const size_t MAX_CODE_SIZE = 4096;


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
    uint32_t  p_type;           // Тип сегмента (PT_LOAD = 1)
    uint32_t  p_flags;          // Флаги (PF_X, PF_W, PF_R)
    uint64_t  p_offset;         // Смещение в файле
    uint64_t  p_vaddr;          // Виртуальный адрес в памяти
    uint64_t  p_paddr;          // Физический адрес (обычно = vaddr)
    uint64_t  p_filesz;         // Размер в файле
    uint64_t  p_memsz;          // Размер в памяти
    uint64_t  p_align;          // Выравнивание
};

struct SectionHeader
{
    uint32_t sh_name;           // смещение в .shstrtab до имени секции    
    uint32_t sh_type;           // тип секции (SHT_PROGBITS, SHT_STRTAB, ...)
    uint64_t sh_flags;          // флаги доступа   
    uint64_t sh_addr;           // адрес в памяти
    uint64_t sh_offset;         // смещение секции в файле    
    uint64_t sh_size;           // размер секции
    uint32_t sh_link;           // зависит от типа (например, .symtab ссылается на .strtab)    
    uint32_t sh_info;           // доп информация, зависит от типа
    uint64_t sh_addralign;      // требуемое выравнивание    
    uint64_t sh_entsize;        // размер элемента, если это таблица
};


const size_t LABEL_LEN = 40;
const size_t MAX_LABEL_NUM = 200;
const size_t MAX_FIXUP_NUM = 200;

const size_t POISON_LABEL_INDX = 0xBABACED;

struct TextLabels
{
    size_t label_nums[MAX_LABEL_NUM];   // номер метки
    size_t label_vals[MAX_LABEL_NUM];   // куда указывает

    size_t size;
};

struct TextFixups
{
    size_t label_addrs[MAX_FIXUP_NUM];  // где встретилось обращение к метке
    size_t label_nums [MAX_FIXUP_NUM];  // номер этой метки

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

    // выравнивание к заголовкам секций (количество нулей)
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