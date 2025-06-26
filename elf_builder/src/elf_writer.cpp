#include <stdio.h>
#include <string.h>

#include "elf_info.h"
#include "elf_debug.h"
#include "logger.h"
#include "byte_code_builder.h"

void ElfHeaderFill(ElfHeader *elf_hdr)
{
    memcpy(elf_hdr->e_ident, "\x7F" "ELF" "\x02" "\x01" "\x01", 7);      // magic numbers
    memset(elf_hdr->e_ident + 7, 0, 9);                                  // the rest are zeros

    elf_hdr->e_type      = 2;                                            // ET_EXEC
    elf_hdr->e_machine   = 0x3E;                                         // x86-64
    elf_hdr->e_version   = 1;
    elf_hdr->e_entry     = 0x400000 + CODE_OFFSET;
    elf_hdr->e_phoff     = sizeof(ElfHeader);
    elf_hdr->e_shoff     = 0;
    elf_hdr->e_flags     = 0;
    elf_hdr->e_ehsize    = sizeof(ElfHeader);
    elf_hdr->e_phentsize = sizeof(ProgramHeader);
    elf_hdr->e_phnum     = 1;
    elf_hdr->e_shentsize = 0;
    elf_hdr->e_shnum     = 0;
    elf_hdr->e_shstrndx  = 0;
}


void ProgramHeaderFill(ProgramHeader *prog_hdr)
{
    prog_hdr->p_type   = 1;                  // PT_LOAD
    prog_hdr->p_flags  = 5;                  // PF_R | PF_X
    prog_hdr->p_offset = 0;
    prog_hdr->p_vaddr  = 0x400000;           // virtual download address
    prog_hdr->p_paddr  = prog_hdr->p_vaddr;
    prog_hdr->p_filesz = 12;                 // FIXME: размер кода не знаем наперед
    prog_hdr->p_memsz  = prog_hdr->p_filesz;
    prog_hdr->p_align  = 0x200000;
}


ElfFuncRes ElfDataCtor(ElfData *elf_data)
{
    lassert(elf_data);

    ElfHeaderFill(&elf_data->elf_hdr);
    ProgramHeaderFill(&elf_data->prog_hdr);

    ELF_HANDLE(GetByteCodeFromIR(NULL, &elf_data->code));
    lassert(elf_data->code.data);

    // TODO: GetCode etc...

    return ELF_FUNC_OK;
}


ElfFuncRes ElfDataDtor(ElfData *elf_data)
{
    lassert(elf_data);

    free(elf_data->code.data);
    elf_data->code.size = 0;

    return ELF_FUNC_OK;
}


ElfFuncRes WriteCode(FILE *elf_file, ElfData *elf_data)
{
    lassert(elf_file);
    lassert(elf_data);

    size_t elf_hdr_write_res = fwrite(&elf_data->elf_hdr, sizeof(elf_data->elf_hdr), 1, elf_file);

    if (elf_hdr_write_res != 1)
    {
        log(ERROR, "elf_hdr_write failed");
        return ELF_FUNC_FAIL;
    }

    size_t prog_hdr_write_res = fwrite(&elf_data->prog_hdr, sizeof(elf_data->prog_hdr), 1, elf_file);
    
    if (prog_hdr_write_res != 1)
    {
        log(ERROR, "prog_hdr_write failed");
        return ELF_FUNC_FAIL;
    }


    char zeros[CODE_OFFSET - sizeof(ElfHeader) - sizeof(ProgramHeader)] = {};
    if (sizeof(zeros) != 0)
    {
        size_t zeros_write_res = fwrite(zeros, sizeof(zeros), 1, elf_file);
        
        fprintf(stderr, "zeros_write_res = %lu\n", zeros_write_res);
        if (zeros_write_res != 1)
        {
            log(ERROR, "zeros_write failed");
            return ELF_FUNC_FAIL;
        }
    }
        

    size_t code_write_res = fwrite(elf_data->code.data, elf_data->code.size, 1, elf_file);

    if (code_write_res != 1)
    {
        log(ERROR, "code_write failed");
        return ELF_FUNC_FAIL;
    }


    return ELF_FUNC_OK;
}