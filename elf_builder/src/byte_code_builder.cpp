#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>


#include "elf_info.h"
#include "ir.h"
#include "byte_code_builder.h"
#include "instructions_to_bytes.h"


static ElfFuncRes AssignBlockToBytes    (const IrBlock *assign_block     , TextSection *text);
static ElfFuncRes OperationBlockToBytes (const IrBlock *operation_block  , TextSection *text);
static ElfFuncRes CondJumpToBytes       (const IrBlock *jump_block       , TextSection *text, CondType cond_type);
static ElfFuncRes CallFuncToBytes       (const IrBlock *call_func_block  , TextSection *text);
static ElfFuncRes FuncReturnToBytes     (const IrBlock *func_return_block, TextSection *text);
static ElfFuncRes SyscallToBytes        (const IrBlock *syscall_block    , TextSection *text);

static void AddOperationInRaxBytes  (TextSection *text, IrOperand operand1, IrOperand operand2);
static void SubOperationInRaxBytes  (TextSection *text, IrOperand operand1, IrOperand operand2);
static void MulOperationInRaxBytes  (TextSection *text, IrOperand operand1, IrOperand operand2);
static void DivOperationInRaxBytes  (TextSection *text, IrOperand operand1, IrOperand operand2);
static void BoolOperationInRaxBytes (TextSection *text, IrOperand operand1, IrOperand operand2, CMovCode cmov_code);
static void SqrtOperationInRaxBytes (TextSection *text, IrOperand operand);


static ElfFuncRes NewLabel          (TextSection *text, size_t new_label_num);
static ElfFuncRes FuncBodyToBytes   (TextSection *text, size_t func_num);

static ElfFuncRes AssignVarToBytes  (IrOperand var_operand, IrOperand source_operand, TextSection *text);
static ElfFuncRes AssignArgToBytes  (IrOperand arg_operand, IrOperand source_operand, TextSection *text);

static void MakePreambleBytes(TextSection *text);

static ElfFuncRes FuncFirstArgToBytes(IrOperand arg_source_operand, TextSection *text);

static void GetOperandValToRegisterBytes (TextSection *text, IrOperand operand, RegCode dest_reg_code);

static void UseFixups (TextSection *text);

static size_t FindLabelIndxByNum (TextLabels *labels, size_t label_num);

static ElfFuncRes AddSystemLib(TextSection *text);

static size_t GetSysFuncNumByName(const char *const sys_name);



ElfFuncRes GetByteCodeFromIR(IR_struct *ir, TextSection *text_sect)
{
    IR_VERIFY(ir, ELF_FUNC_FAIL);
    lassert(text_sect);

    const IrBlock *blocks = ir->blocks;

    MakePreambleBytes(text_sect);

    for (size_t i = 0; i < ir->size; i++)
        ELF_HANDLE(IrBlockToBytes(blocks + i, text_sect));

    UseFixups(text_sect);


    log(INFO, "Res code:\n");
    for (size_t i = 0; i < text_sect->size; i++)
        fprintf(stderr, "%02x ", (unsigned char) text_sect->code[i]);

    IR_VERIFY(ir, ELF_FUNC_FAIL);
    return ELF_FUNC_OK;
}


static void MakePreambleBytes(TextSection *text)
{
    lassert(text);

    memcpy(text->code + text->size, START_PREAMBLE, START_PREAMBLE_LEN);    // start preamble
    text->size += START_PREAMBLE_LEN;

    // main func call
    Label main_func_label = {.func = {.num = MAIN_FUNC_NUM, .arg_cnt = MAIN_FUNC_ARG_CNT}};
    IrOperand main_arg_fictive_operand = {.type = IR_OPERAND_TYPE_NUM, .val = {.num = 0}};

    IrBlock call_main_fictive_block = {
        .block_type_info = GetIrBlockTypeInfo(IR_BLOCK_TYPE_CALL_FUNCTION), 
        .operand_1 = {.type = IR_OPERAND_TYPE_FUNC_LABEL, .val = {.label = main_func_label}}
    };

    FuncFirstArgToBytes(main_arg_fictive_operand, text);
    CallFuncToBytes(&call_main_fictive_block, text);
    
    memcpy(text->code + text->size, EXIT_PREAMBLE, EXIT_PREAMBLE_LEN);      // hlt
    text->size += EXIT_PREAMBLE_LEN;

    ELF_HANDLE(AddSystemLib(text));

    text->size += 16;   // FIXME: убрать
}

static ElfFuncRes AddSystemLib(TextSection *text)
{
    int lib_descriptor = open(SYSTEM_LIB_BIN_FULLNAME, O_RDONLY);

    if (lib_descriptor == -1)
    {
        log(ERROR, "error opening system_lib_bin");
        return ELF_FUNC_FAIL;
    }

    struct stat lib_stat;

    if (fstat(lib_descriptor, &lib_stat) == -1)
    {
        log(ERROR, "fstat failed");
        close(lib_descriptor);
        return ELF_FUNC_FAIL;
    }

    size_t lib_size = (size_t) lib_stat.st_size;
    if (lib_size == 0) 
    {
        log(WARNING, "System lib file is empty\n");
        close(lib_descriptor);
        return ELF_FUNC_OK;
    }

    char* lib_data = (char *) mmap (
        NULL,
        lib_size,
        PROT_READ,
        MAP_PRIVATE,
        lib_descriptor,
        0
    );

    if (lib_data == MAP_FAILED) 
    {
        log(ERROR, "mmap failed");
        close(lib_descriptor);
        return ELF_FUNC_FAIL;
    }

    memcpy(text->code + text->size, lib_data, lib_size);

    TextLabels *labels = &text->labels;

    size_t labels_size = labels->size++;
    labels->label_nums[labels_size] = SYS_PRINTF_NUM;
    labels->label_vals[labels_size] = text->size + SYS_PRINTF_SHIFT;

    labels_size = labels->size++;
    labels->label_nums[labels_size] = SYS_SCANF_NUM;
    labels->label_vals[labels_size] = text->size + SYS_SCANF_SHIFT;

    labels_size = labels->size++;
    labels->label_nums[labels_size] = SYS_SQRT_NUM;
    labels->label_vals[labels_size] = text->size + SYS_SQRT_SHIFT;

    text->size += lib_size;


    if (munmap(lib_data, lib_size) == -1) 
    {
        log(ERROR, "munmap failed");
        close(lib_descriptor);
        return ELF_FUNC_FAIL;
    }

    close(lib_descriptor);
    return ELF_FUNC_OK;
}


ElfFuncRes IrBlockToBytes(const IrBlock *block, TextSection *text)
{
    lassert(block);
    lassert(text);
    
    switch (block->block_type_info->type)
    {
    case IR_BLOCK_TYPE_ASSIGNMENT    :  return AssignBlockToBytes    (block, text); 
    case IR_BLOCK_TYPE_OPERATION     :  return OperationBlockToBytes (block, text);
    case IR_BLOCK_TYPE_LOCAL_LABEL   :  return NewLabel              (text,  block->label.local);
    case IR_BLOCK_TYPE_COND_JUMP     :  return CondJumpToBytes       (block, text, DO_IF_TRUE);
    case IR_BLOCK_TYPE_NEG_COND_JUMP :  return CondJumpToBytes       (block, text, DO_IF_FALSE);
    case IR_BLOCK_TYPE_CALL_FUNCTION :  return CallFuncToBytes       (block, text);
    case IR_BLOCK_TYPE_FUNCTION_BODY :  return FuncBodyToBytes       (text,  block->label.func.num);
    case IR_BLOCK_TYPE_RETURN        :  return FuncReturnToBytes     (block, text);
    case IR_BLOCK_TYPE_SYSCALL       :  return SyscallToBytes        (block, text);
    case IR_BLOCK_TYPE_INVALID       :

    default                          : return ELF_FUNC_FAIL;
    }
}

static ElfFuncRes AssignBlockToBytes(const IrBlock *assign_block, TextSection *text)
{
    lassert(assign_block && assign_block->block_type_info->type == IR_BLOCK_TYPE_ASSIGNMENT);
    lassert(text);

    IrOperand dest_operand   = assign_block->operand_1;
    IrOperand source_operand = assign_block->operand_2;

    if (dest_operand.type == IR_OPERAND_TYPE_VAR)
        return AssignVarToBytes(dest_operand, source_operand, text);

    else    // if type == arg
        return AssignArgToBytes(dest_operand, source_operand, text);

    return ELF_FUNC_OK;
}

static ElfFuncRes AssignVarToBytes(IrOperand var_operand, IrOperand source_operand, TextSection *text)
{
    lassert(var_operand.type == IR_OPERAND_TYPE_VAR);
    lassert(source_operand.type == IR_OPERAND_TYPE_NUM || source_operand.type == IR_OPERAND_TYPE_TMP);
    lassert(text);

    log(INFO, "var_num = %lu, num = %d", var_operand.val.var, source_operand.val.num);

    if (source_operand.type == IR_OPERAND_TYPE_TMP)
    {
        PopReg(text, RAX_CODE);
        MovVarReg(text, (int) var_operand.val.var, RAX_CODE);
    }

    else    // if type == num
        MovVarImm(text, var_operand.val.var, source_operand.val.num);
        
    SubRegImm(text, RSP_CODE, 8);

    return ELF_FUNC_OK;
}


static ElfFuncRes AssignArgToBytes(IrOperand arg_operand, IrOperand source_operand, TextSection *text)
{
    lassert(arg_operand.type == IR_OPERAND_TYPE_ARG);
    lassert(source_operand.type == IR_OPERAND_TYPE_NUM || source_operand.type == IR_OPERAND_TYPE_TMP || source_operand.type == IR_OPERAND_TYPE_VAR);
    lassert(text);

    if (arg_operand.val.arg == 0)   // the first arg of func call -> free space for future ret addr
        return FuncFirstArgToBytes(source_operand, text);

    if (source_operand.type == IR_OPERAND_TYPE_NUM)
        PushImm(text, source_operand.val.num);

    else if (source_operand.type == IR_OPERAND_TYPE_VAR)
        PushVar(text, source_operand.val.var);

    // else argument is inited by tmp that is already in stack

    return ELF_FUNC_OK;
}

static ElfFuncRes FuncFirstArgToBytes(IrOperand arg_source_operand, TextSection *text)
{
    if (arg_source_operand.type == IR_OPERAND_TYPE_NUM)
    {
        SubRegImm(text, RSP_CODE, 8);
        PushReg  (text, RBP_CODE);
        PushImm  (text, arg_source_operand.val.num);
    }

    else if (arg_source_operand.type == IR_OPERAND_TYPE_TMP)
    {
        PopReg   (text, RAX_CODE);
        SubRegImm(text, RSP_CODE, 8);
        PushReg  (text, RBP_CODE);
        PushReg  (text, RAX_CODE);
    }

    else // type == IR_OPERAND_VAR
    {
        SubRegImm(text, RSP_CODE, 8);
        PushReg(text, RBP_CODE);
        PushVar(text, arg_source_operand.val.var);
    }

    MovRegReg(text, RAX_CODE, RSP_CODE);

    return ELF_FUNC_OK;
}



static ElfFuncRes OperationBlockToBytes(const IrBlock *operation_block, TextSection *text)
{
    lassert(operation_block && operation_block->block_type_info->type == IR_BLOCK_TYPE_OPERATION);
    lassert(text);

    IrOperand operand1 = operation_block->operand_1;
    IrOperand operand2 = operation_block->operand_2;

    switch (operation_block->operation_info->type)
    {
    case IR_OPERATION_TYPE_EQ      :  BoolOperationInRaxBytes(text, operand1, operand2, CMOV_E) ; break;
    case IR_OPERATION_TYPE_NEQ     :  BoolOperationInRaxBytes(text, operand1, operand2, CMOV_NE); break;
    case IR_OPERATION_TYPE_LESS    :  BoolOperationInRaxBytes(text, operand1, operand2, CMOV_L) ; break;
    case IR_OPERATION_TYPE_LESSEQ  :  BoolOperationInRaxBytes(text, operand1, operand2, CMOV_LE); break;
    case IR_OPERATION_TYPE_GREAT   :  BoolOperationInRaxBytes(text, operand1, operand2, CMOV_G) ; break;
    case IR_OPERATION_TYPE_GREATEQ :  BoolOperationInRaxBytes(text, operand1, operand2, CMOV_GE); break;

    case IR_OPERATION_TYPE_ADD     :  AddOperationInRaxBytes (text, operand1, operand2);          break;
    case IR_OPERATION_TYPE_SUB     :  SubOperationInRaxBytes (text, operand1, operand2);          break;
    case IR_OPERATION_TYPE_MUL     :  MulOperationInRaxBytes (text, operand1, operand2);          break;
    case IR_OPERATION_TYPE_DIV     :  DivOperationInRaxBytes (text, operand1, operand2);          break;
    case IR_OPERATION_TYPE_SQRT    :  SqrtOperationInRaxBytes(text, operand1);                    break;

    case IR_OPERATION_TYPE_POW     :
    case IR_OPERATION_TYPE_NONE    :
    default                        :    return ELF_FUNC_FAIL;
    }

    PushReg(text, RAX_CODE);

    return ELF_FUNC_OK;
}


static ElfFuncRes NewLabel(TextSection *text, size_t new_label_num)
{
    lassert(text);

    size_t labels_size = text->labels.size++;

    text->labels.label_nums[labels_size] = new_label_num;
    text->labels.label_vals[labels_size] = text->size;

    return ELF_FUNC_OK;
}


static ElfFuncRes CondJumpToBytes(const IrBlock *jump_block, TextSection *text, CondType cond_type)
{
    lassert(jump_block);
    lassert(jump_block->block_type_info->type == IR_BLOCK_TYPE_COND_JUMP || jump_block->block_type_info->type == IR_BLOCK_TYPE_NEG_COND_JUMP);

    IrOperand label_operand     = jump_block->operand_1;
    IrOperand condition_operand = jump_block->operand_2;

    lassert(label_operand.type     == IR_OPERAND_TYPE_LOCAL_LABEL, "label num = '%d'", label_operand.val);
    lassert(condition_operand.type == IR_OPERAND_TYPE_TMP || condition_operand.type == IR_OPERAND_TYPE_NUM);

    if (condition_operand.type == IR_OPERAND_TYPE_NUM)
    {
        if (condition_operand.val.num != 0)
            FictitiousJump(text, DO_ANYWAY, label_operand.val.label.local);

        return ELF_FUNC_OK;
    }

    GetOperandValToRegisterBytes(text, condition_operand, RAX_CODE);

    TestRaxRax(text);

    FictitiousJump(text, cond_type, label_operand.val.label.local);

    return ELF_FUNC_OK;
}


static ElfFuncRes CallFuncToBytes(const IrBlock *call_func_block, TextSection *text)
{
    lassert(call_func_block && call_func_block->block_type_info->type == IR_BLOCK_TYPE_CALL_FUNCTION);
    lassert(text);

    // stack frame debug in FuncFirstArgToBytes()
    
    IrOperand func_label = call_func_block->operand_1;
    lassert(func_label.type == IR_OPERAND_TYPE_FUNC_LABEL);

    MovRegReg(text, RBP_CODE, RAX_CODE);

    FictitiousCall(text, func_label.val.label.func.num);

    PushReg(text, RAX_CODE);    // ret val in stack

    // ret_addr to space before args  in FuncBodyToAsm()

    return ELF_FUNC_OK;
}


static ElfFuncRes FuncBodyToBytes(TextSection *text, size_t func_num)
{
    lassert(text);

    NewLabel(text, func_num);

    PopReg(text, RAX_CODE);

    MovVarReg(text, -2, RAX_CODE);  // mov [rbp + 2 * 8], rax
                                    // rax = ret_addr -> before saved rbp (rbp is pointing on arg_0)

    return ELF_FUNC_OK;
}


static ElfFuncRes FuncReturnToBytes(const IrBlock *func_return_block, TextSection *text)
{
    lassert(func_return_block && func_return_block->block_type_info->type == IR_BLOCK_TYPE_RETURN);
    lassert(text);

    IrOperand ret_operand = func_return_block->ret_operand;

    GetOperandValToRegisterBytes(text, ret_operand, RAX_CODE);

    MovRegReg(text, RSP_CODE, RBP_CODE);    // mov rsp, rbp
    AddRegImm(text, RSP_CODE, 8);           // add rsp, 8
    PopReg   (text, RBP_CODE);              // pop rbp
    FuncRet  (text);                        // ret

    return ELF_FUNC_OK;
}

static ElfFuncRes SyscallToBytes(const IrBlock *syscall_block, TextSection *text)
{
    lassert(syscall_block && syscall_block->block_type_info->type == IR_BLOCK_TYPE_SYSCALL);
    lassert(text);

    if (syscall_block->operand_1.type != IR_OPERAND_TYPE_NONE)
        GetOperandValToRegisterBytes(text, syscall_block->operand_1, RAX_CODE);  // syscall arg in rax

    size_t sys_func_num = GetSysFuncNumByName(syscall_block->label.sys);
    FictitiousCall(text, sys_func_num);

    if (syscall_block->ret_operand.type != IR_OPERAND_TYPE_NONE)
        PushReg(text, RAX_CODE);

    return ELF_FUNC_OK;
}


static size_t GetSysFuncNumByName(const char *const sys_name)
{
    if (strcmp(sys_name, SYS_PRINTF_NAME) == 0)
        return SYS_PRINTF_NUM;

    else if (strcmp(sys_name, SYS_SCANF_NAME) == 0)
        return SYS_SCANF_NUM;

    else 
    {
        log(ERROR, "unknown sys func name: '%s'", sys_name);
        return (size_t) -1;
    }
}


static void AddOperationInRaxBytes(TextSection *text, IrOperand operand1, IrOperand operand2)
{
    lassert(text);

    GetOperandValToRegisterBytes(text, operand2, RBX_CODE);
    GetOperandValToRegisterBytes(text, operand1, RAX_CODE);

    AddRegReg(text, RAX_CODE, RBX_CODE);
}

static void SubOperationInRaxBytes(TextSection *text, IrOperand operand1, IrOperand operand2)
{
    lassert(text);

    GetOperandValToRegisterBytes(text, operand2, RBX_CODE);
    GetOperandValToRegisterBytes(text, operand1, RAX_CODE);

    SubRegReg(text, RAX_CODE, RBX_CODE);
}

static void MulOperationInRaxBytes(TextSection *text, IrOperand operand1, IrOperand operand2)
{
    lassert(text);

    GetOperandValToRegisterBytes(text, operand2, RBX_CODE);
    GetOperandValToRegisterBytes(text, operand1, RAX_CODE);

    MulReg(text, RBX_CODE);
}

static void DivOperationInRaxBytes(TextSection *text, IrOperand operand1, IrOperand operand2)
{
    lassert(text);

    XorRegReg(text, RDX_CODE, RDX_CODE);

    GetOperandValToRegisterBytes(text, operand2, RBX_CODE);
    GetOperandValToRegisterBytes(text, operand1, RAX_CODE);

    ConvQuadwToOctow(text);     // rax -> rdx:rax

    IDivReg(text, RBX_CODE);
}

static void SqrtOperationInRaxBytes(TextSection *text, IrOperand operand)
{
    lassert(text);

    GetOperandValToRegisterBytes(text, operand, RBX_CODE);

    FictitiousCall(text, SYS_SQRT_NUM);
}


static void BoolOperationInRaxBytes(TextSection *text, IrOperand operand1, IrOperand operand2, CMovCode cmov_code)
{
    lassert(text);

    GetOperandValToRegisterBytes(text, operand2, RBX_CODE);
    GetOperandValToRegisterBytes(text, operand1, RAX_CODE);

    CmpRegReg(text, RAX_CODE, RBX_CODE);
    MovRegImm(text, RAX_CODE, 0);
    MovRegImm(text, RBX_CODE, 1);

    CMovRegReg(text, RAX_CODE, RBX_CODE, cmov_code);
}




static void GetOperandValToRegisterBytes(TextSection *text, IrOperand operand, RegCode dest_reg_code)
{
    lassert(text);
    lassert(operand.type == IR_OPERAND_TYPE_NUM || operand.type == IR_OPERAND_TYPE_VAR || operand.type == IR_OPERAND_TYPE_TMP);

    if (operand.type == IR_OPERAND_TYPE_TMP)
        PopReg(text, dest_reg_code);

    else if (operand.type == IR_OPERAND_TYPE_VAR)
        MovRegVar(text, dest_reg_code, operand.val.var);

    else // if type == NUM
        MovRegImm(text, dest_reg_code, operand.val.num);
}

static void UseFixups(TextSection *text)
{
    lassert(text);

    TextFixups fixups = text->fixups;
    TextLabels labels = text->labels;

    for (size_t i = 0; i < fixups.size; i++)
    {
        size_t label_addr = fixups.label_addrs[i];
        size_t label_num  = fixups.label_nums [i];

        size_t label_indx = FindLabelIndxByNum(&labels, label_num);

        lassert(labels.label_nums[label_indx] == label_num);    // проверка на всякий случай
        size_t label_val = labels.label_vals[label_indx];

        int jmp_shift = (int) label_val - (int) label_addr - 4; // 4 = sizeof(смешение)

        memcpy(text->code + label_addr, &jmp_shift, 4);
    }
}


static size_t FindLabelIndxByNum(TextLabels *labels, size_t label_num)
{
    for (size_t i = 0; i < labels->size; i++)
    {
        if (labels->label_nums[i] == label_num)
            return i;
    }

    lassert(ERROR, "Error finding label");
    return POISON_LABEL_INDX;
}