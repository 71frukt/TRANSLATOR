#ifndef PYAM_IR_SRC_OPERATIONS_H
#define PYAM_IR_SRC_OPERATIONS_H

enum IrOperandType
{
    IR_OPERAND_TYPE_NONE        = 1 << 0,
    IR_OPERAND_TYPE_NUM         = 1 << 1,
    IR_OPERAND_TYPE_TMP         = 1 << 2,
    IR_OPERAND_TYPE_VAR         = 1 << 3,
    IR_OPERAND_TYPE_ARG         = 1 << 4,
    IR_OPERAND_TYPE_LOCAL_LABEL = 1 << 5,
    IR_OPERAND_TYPE_FUNC_LABEL  = 1 << 6,
};


enum IrOperationType
{
    IR_OPERATION_TYPE_ADD     ,
    IR_OPERATION_TYPE_SUB     ,
    IR_OPERATION_TYPE_MUL     ,
    IR_OPERATION_TYPE_DIV     ,
    IR_OPERATION_TYPE_SQRT    ,
    IR_OPERATION_TYPE_POW     ,
    IR_OPERATION_TYPE_EQ      ,
    IR_OPERATION_TYPE_NEQ     ,
    IR_OPERATION_TYPE_LESS    ,
    IR_OPERATION_TYPE_LESSEQ  ,
    IR_OPERATION_TYPE_GREAT   ,
    IR_OPERATION_TYPE_GREATEQ ,
    IR_OPERATION_TYPE_NONE    ,
};


enum IrBlockType
{
    IR_BLOCK_TYPE_CALL_FUNCTION ,
    IR_BLOCK_TYPE_FUNCTION_BODY ,
    IR_BLOCK_TYPE_COND_JUMP     ,
    IR_BLOCK_TYPE_NEG_COND_JUMP ,
    IR_BLOCK_TYPE_ASSIGNMENT    ,
    IR_BLOCK_TYPE_OPERATION     ,
    IR_BLOCK_TYPE_RETURN        ,
    IR_BLOCK_TYPE_LOCAL_LABEL   ,
    IR_BLOCK_TYPE_SYSCALL       ,
    IR_BLOCK_TYPE_INVALID       ,
};

////--------------------------------------------------------------------------------------------------------------------------------////
// ||    Default name        ||     Return value    ||  Operation  ||     Label    ||     First Operand     ||    Second Operand    ||
// ||  Call function         ||  Tmp                ||  None       ||  None        ||  Func label           ||  None                ||
// ||  Function body         ||  None               ||  None       ||  Func label  ||  Number of arguments  ||  None                ||
// || (neg) Conditional jump ||  None               ||  None       ||  None        ||  Local label          ||  Tmp                 ||
// ||  Assignment            ||  Tmp or var         ||  None       ||  None        ||  Var or Tmp           ||  Var or Tmp or Num   ||
// ||  Operation             ||  Tmp or var         ||  operation  ||  None        ||  Var or Tmp or Num    ||  Var or Tmp or Num   ||
// ||  Local label           ||  None               ||  None       ||  Local Label ||  None                 ||  None                ||
// ||  Return                ||  Tmp or var or Num  ||  None       ||  None        ||  None                 ||  None                ||
// ||  Call sys function     ||  Tmp                ||  None       ||  Func label  ||  Var or Tmp or Num    ||  None                ||
////--------------------------------------------------------------------------------------------------------------------------------////


#endif
