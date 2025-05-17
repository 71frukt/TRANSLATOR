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
    // IR_OPERAND_TYPE_OPERATION   = 1 << 7,
};


enum IrOperationType
{
    IR_OPERATION_TYPE_SUM     ,
    IR_OPERATION_TYPE_SUB     ,
    IR_OPERATION_TYPE_MUL     ,
    IR_OPERATION_TYPE_DIV     ,
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
    IR_BLOCK_TYPE_CALL_FUNCTION , // Call function                                       :
    IR_BLOCK_TYPE_FUNCTION_BODY , // Function body description (serves as label as well) :
    IR_BLOCK_TYPE_COND_JUMP     , // Conditional jump                                    :
    IR_BLOCK_TYPE_ASSIGNMENT    , // Assignment                                          :
    IR_BLOCK_TYPE_OPERATION     , // Operation                                           :
    IR_BLOCK_TYPE_RETURN        , // Return                                              :
    IR_BLOCK_TYPE_LOCAL_LABEL   , // Local label                                         :
    IR_BLOCK_TYPE_SYSCALL       , // System function call                                :
    IR_BLOCK_TYPE_INVALID       ,
};

////                                               PYAM_IR table (PYAMIRT) for better understanding                              ////
////-----------------------------------------------------------------------------------------------------------------------------////
// ||    Default name    ||     Return value    ||  Operation  ||     Label    ||     First Operand     ||    Second Operand    ||
// ||  Call function     ||  Tmp                ||  None       ||  Func label  ||  None                 ||  None                ||
// ||  Function body     ||  None               ||  None       ||  Func label  ||  Number of arguments  ||  None                ||
// ||  Conditional jump  ||  None               ||  None       ||  None        ||  Local label          ||  Tmp                 ||
// ||  Assignment        ||  Tmp or var         ||  None       ||  None        ||  Var or Tmp           ||  Var or Tmp or Num   ||
// ||  Operation         ||  Tmp or var         ||  operation  ||  None        ||  Var or Tmp or Num    ||  Var or Tmp or Num   ||
// ||  Local label       ||  None               ||  None       ||  Local Label ||  None                 ||  None                ||
// ||  Return            ||  Tmp or var or Num  ||  None       ||  None        ||  None                 ||  None                ||
// ||  Call sys function ||  None               ||  None       ||  None        ||  None                 ||  None                ||
////-----------------------------------------------------------------------------------------------------------------------------////
////                                                        End of PYAM_IR table (EOPYAMIRT)                                     ////

#endif
