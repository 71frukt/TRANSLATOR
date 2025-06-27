#include <stdio.h>
#include <string.h>

#include "elf_info.h"
#include "elf_debug.h"
#include "logger.h"
#include "byte_code_builder.h"

void ElfHeaderFill(ElfData *elf_data)   // здесь уже должны быть заполнены Section Header
{
    lassert(elf_data);

    ElfHeader *elf_hdr = &elf_data->elf_hdr;

    memcpy(elf_hdr->e_ident, "\x7F" "ELF" "\x02" "\x01" "\x01", 7);      // magic numbers
    memset(elf_hdr->e_ident + 7, 0, 9);                                  // the rest are zeros

    elf_hdr->e_type      = 2;                                            // ET_EXEC
    elf_hdr->e_machine   = 0x3E;                                         // x86-64
    elf_hdr->e_version   = 1;
    elf_hdr->e_entry     = ENTRY_ADDR + CODE_OFFSET;
    elf_hdr->e_phoff     = sizeof(ElfHeader);

    elf_data->sect_hdrs_allign = elf_data->text_sect_hdr.sh_addralign - (elf_data->shstrtab_hdr.sh_offset + elf_data->shstrtab_hdr.sh_size) % SECT_HDR_ALLIGN;

    log(INFO, "elf_data->sect_hdrs_allign = %lu; SECT_HDR_ALLIGN = %lu", elf_data->sect_hdrs_allign, SECT_HDR_ALLIGN);

    if (elf_data->sect_hdrs_allign == SECT_HDR_ALLIGN)
        elf_data->sect_hdrs_allign = 0;
        
    elf_hdr->e_shoff     = elf_data->shstrtab_hdr.sh_offset + elf_data->shstrtab_hdr.sh_size + elf_data->sect_hdrs_allign;
    
    log(INFO, "elf_hdr->e_shoff = %x", elf_hdr->e_shoff);

    elf_hdr->e_flags     = 0;
    elf_hdr->e_ehsize    = sizeof(ElfHeader);
    elf_hdr->e_phentsize = sizeof(ProgramHeader);
    elf_hdr->e_phnum     = 1;
    elf_hdr->e_shentsize = sizeof(SectionHeader);
    elf_hdr->e_shnum     = 3;
    elf_hdr->e_shstrndx  = 2;   // .shstrtab - вторая 
}


void ProgramHeaderFill(ElfData *elf_data)
{
    lassert(elf_data);

    ProgramHeader *prog_hdr = &elf_data->prog_hdr;

    prog_hdr->p_type   = 1;                  // PT_LOAD
    prog_hdr->p_flags  = 5;                  // PF_R | PF_X
    prog_hdr->p_offset = 0;
    prog_hdr->p_vaddr  = ENTRY_ADDR;         // virtual download address
    prog_hdr->p_paddr  = prog_hdr->p_vaddr;
    prog_hdr->p_filesz = elf_data->text_sect.size;
    prog_hdr->p_memsz  = prog_hdr->p_filesz;
    prog_hdr->p_align  = 0x200000;
}


void TextSectionHeaderFill(ElfData *elf_data)
{
    lassert(elf_data);

    SectionHeader *text_sect_hdr = &elf_data->text_sect_hdr;

    text_sect_hdr->sh_name      = 0x01;
    text_sect_hdr->sh_type      = 0x01;
    text_sect_hdr->sh_flags     = 0x6;
    text_sect_hdr->sh_addr      = ENTRY_ADDR + CODE_OFFSET;
    text_sect_hdr->sh_offset    = CODE_OFFSET;
    text_sect_hdr->sh_size      = elf_data->text_sect.size;
    text_sect_hdr->sh_link      = 0;
    text_sect_hdr->sh_info      = 0;
    text_sect_hdr->sh_addralign = 0x10;
    text_sect_hdr->sh_entsize   = 0;
}


void ShstrtabSectionHeaderFill(ElfData *elf_data)
{
    lassert(elf_data);

    SectionHeader *shstrtab_hdr = &elf_data->shstrtab_hdr;

    shstrtab_hdr->sh_name      = 0x07;
    shstrtab_hdr->sh_type      = 0x03;
    shstrtab_hdr->sh_flags     = 0x6;

    shstrtab_hdr->sh_offset    = elf_data->text_sect_hdr.sh_offset + elf_data->text_sect.size;
    shstrtab_hdr->sh_addr      = ENTRY_ADDR + shstrtab_hdr->sh_offset;

    shstrtab_hdr->sh_size      = 0x11;
    shstrtab_hdr->sh_link      = 0;
    shstrtab_hdr->sh_info      = 0;
    shstrtab_hdr->sh_addralign = 0x1;
    shstrtab_hdr->sh_entsize   = 0;
}



ElfFuncRes ElfDataFill(ElfData *elf_data)
{
    lassert(elf_data);

    ELF_HANDLE(GetByteCodeFromIR(NULL, &elf_data->text_sect));  // должно быть первым, тк здесь узнаем размер кода!
    lassert(elf_data->text_sect.code);
    
    TextSectionHeaderFill(elf_data);

    ShstrtabSectionHeaderFill(elf_data);
    elf_data->shstrtab = SHSTRTAB_ELF;
    
    ProgramHeaderFill(elf_data);
    ElfHeaderFill(elf_data);

    return ELF_FUNC_OK;
}


// ElfFuncRes ElfDataDtor(ElfData *elf_data)
// {
//     lassert(elf_data);

//     elf_data->text_sect.size = 0;

//     return ELF_FUNC_OK;
// }


ElfFuncRes WriteCode(FILE *elf_file, ElfData *elf_data)
{
    lassert(elf_file);
    lassert(elf_data);

    fwrite(&elf_data->elf_hdr, sizeof(ElfHeader), 1, elf_file);

    fwrite(&elf_data->prog_hdr, sizeof(ProgramHeader), 1, elf_file);

    char zeros[CODE_OFFSET - sizeof(ElfHeader) - sizeof(ProgramHeader)] = {};
    if (sizeof(zeros) != 0)
        fwrite(zeros, sizeof(zeros), 1, elf_file);
        
    fwrite(elf_data->text_sect.code, elf_data->text_sect.size, 1, elf_file);

    fwrite(elf_data->shstrtab, elf_data->shstrtab_hdr.sh_size, 1, elf_file);

    // Section Headers
    // ...alignment
    size_t sect_hdrs_allign_num = elf_data->sect_hdrs_allign;
    
    log(INFO, "sect_hdrs_allign_num = %lu", sect_hdrs_allign_num);

    char *sect_allign_zeros = (char *) calloc(sect_hdrs_allign_num, sizeof(char));
    fwrite(sect_allign_zeros, 1, sect_hdrs_allign_num, elf_file);
    free(sect_allign_zeros);

    fwrite(&elf_data->null_header  , sizeof(SectionHeader), 1, elf_file);
    fwrite(&elf_data->text_sect_hdr, sizeof(SectionHeader), 1, elf_file);
    fwrite(&elf_data->shstrtab_hdr , sizeof(SectionHeader), 1, elf_file);

    return ELF_FUNC_OK;
}