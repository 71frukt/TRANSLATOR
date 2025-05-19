#ifndef NASM_OPERATIONS_H
#define NASM_OPERATIONS_H

#define RAX_  "rax "
#define RBX_  "rbx "
#define RBP_  "rbp "
#define RSP_  "rsp "

#define _VAR_          "[" RBP_ "+ %lu]"
#define _NUM_          "%d "
#define _FUNC_         "func_%lu_%lu "
#define _LOCAL_LABEL_  "local_%lu "
#define _FUNC_INIT_    _FUNC_ ": \n"

#define _DEF_LOCAL_LABEL_  _LOCAL_LABEL_ ":\n"

#define PUSH_SYM  "push "
#define POP_SYM   "pop "

#define MOV_SYM  "mov "
#define CMP_SYM  "cmp "

#define MUL_SYM  "mul "
#define ADD_SYM  "add "
#define SUB_SYM  "sub "

#define CMOVE_SYM   "cmove "
#define CMOVNE_SYM  "cmovne "
#define CMOVG_SYM   "cmovg "
#define CMOVGE_SYM  "cmovge "
#define CMOVL_SYM   "cmovl "
#define CMOVLE_SYM  "cmovle "

#define JMP_SYM  "jmp "
#define JE_SYM   "jz "
#define JNE_SYM  "jnz "

#define TEST_SYM  "test "

#define QWORD_SYM  "qword"


#define PUSH_(source_)        "\t" PUSH_SYM source_ "\n"
#define POP_(dest)            "\t" POP_SYM  dest   "\n"

#define MOV_(dest_, source_)  "\t" MOV_SYM dest_  ", " source_ "\n"
#define CMP_(arg_1_, arg_2_)  "\t" CMP_SYM arg_1_ ", " arg_2_  "\n"

#define MUL_(arg_)             "\t" MUL_SYM arg_ "\n"
#define ADD_(arg_1_, arg_2_)   "\t" ADD_SYM arg_1_ ", " arg_2_ "\n"
#define SUB_(arg_1_, arg_2_)   "\t" SUB_SYM arg_1_ ", " arg_2_ "\n"

#define TEST_(arg_1_, arg_2_)  "\t" TEST_SYM arg_1_ ", " arg_2_ "\n"

#define RET_  "\tret \n\n"
#define CALL_ "\tcall " _FUNC_ "\n"

#endif /*NASM_OPERATIONS_H*/