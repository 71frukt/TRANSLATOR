#ifndef ELF_DEBUG_H
#define ELF_DEBUG_H

enum ElfFuncRes
{
    ELF_FUNC_OK,
    ELF_FUNC_FAIL
};

#define ELF_HANDLE(elf_func) do                                                                                             \
{                                                                                                                           \
    ElfFuncRes func_res = elf_func;                                                                                          \
    if (func_res != ELF_FUNC_OK)                                                                                            \
    {                                                                                                                       \
        log(ERROR, "Failed " #elf_func);                                                                                    \
    }                                                                                                                       \
} while (0)


#endif /*ELF_DEBUG_H*/