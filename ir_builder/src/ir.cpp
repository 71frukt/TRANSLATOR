#include <stdio.h>
#include <string.h>

#include "ir.h"

// static IrOperand NodeToIrOperand(IR_struct *ir, const Node *const cur_node);

static IrOperationType TreeToIrMathOp(MathOperation_enum cur_tree_math_op);

static IrOperand GetIrOperation  (IR_struct *ir, const Node *const math_op_node  );
static IrOperand GetIrNum        (IR_struct *ir, const Node *const num_node      );
static IrOperand GetIrKeyWord    (IR_struct *ir, const Node *const num_node      );
static IrOperand GetIrIf         (IR_struct *ir, const Node *const while_node    );
static IrOperand GetIrWhile      (IR_struct *ir, const Node *const math_op_node  );
static IrOperand GetIrVar        (IR_struct *ir, const Node *const var_node      );
static IrOperand GetIrVarInit    (IR_struct *ir, const Node *const var_init_node );
static IrOperand GetIrAssign     (IR_struct *ir, const Node *const assign_node   );
static IrOperand GetIrFuncInit   (IR_struct *ir, const Node *const func_init_node);
static IrOperand GetFuncReturn   (IR_struct *ir, const Node *const ret_node      );
static IrOperand GetCallParam    (IR_struct *ir, const Node *const param_node    );
static IrOperand GetCallFunc     (IR_struct *ir, const Node *const call_node     );
static IrOperand GetSysCallScan  (IR_struct *ir, const Node *const sys_func_node );
static IrOperand GetSysCallPrint (IR_struct *ir, const Node *const sys_func_node );

static IrOperand IrNewTmpOperand (IR_struct *ir);

static IrBlock *NewAssignIrBlock   (IR_struct *ir, IrOperand dest, IrOperand source);
static IrBlock *NewLocalLabelBlock (IR_struct *ir);
static IrBlock *NewCndJumpIrBlock  (IR_struct *ir, IrOperand jmp_label, IrOperand condition);
static IrBlock *NewJumpIrBlock     (IR_struct *ir, IrOperand jmp_label);
static IrBlock *NewJzIrBlock       (IR_struct *ir, IrOperand jmp_label, IrOperand condition);

static void SetPoisonOperationBlock(IrBlock *operation_block);


#define NONE_OPERAND  {IR_OPERAND_TYPE_NONE, {}}
#define MAIN_FUNC_NAME  "MAIN_HOI"


// ====================== BLOCKS TYPE INFO ===============================          // TODO: переделать нахуй.
#define INIT_BLOCK_TYPE_INFO_(block_type_, debug_name_)   \
    const static IrBlockTypeInfo struct_info_##block_type_ = {block_type_ ON_IR_DEBUG( , debug_name_)}

    //                            block_type               debug_name
    INIT_BLOCK_TYPE_INFO_( IR_BLOCK_TYPE_CALL_FUNCTION , "call_func"     );    
    INIT_BLOCK_TYPE_INFO_( IR_BLOCK_TYPE_FUNCTION_BODY , "func"          );
    INIT_BLOCK_TYPE_INFO_( IR_BLOCK_TYPE_COND_JUMP     , "cond_jump"     );
    INIT_BLOCK_TYPE_INFO_( IR_BLOCK_TYPE_NEG_COND_JUMP , "neg_cond_jump" );    
    INIT_BLOCK_TYPE_INFO_( IR_BLOCK_TYPE_ASSIGNMENT    , "assign"        );
    INIT_BLOCK_TYPE_INFO_( IR_BLOCK_TYPE_OPERATION     , "operation"     );
    INIT_BLOCK_TYPE_INFO_( IR_BLOCK_TYPE_RETURN        , "return"        );
    INIT_BLOCK_TYPE_INFO_( IR_BLOCK_TYPE_LOCAL_LABEL   , "local_label"   );
    INIT_BLOCK_TYPE_INFO_( IR_BLOCK_TYPE_SYSCALL       , "syscall"       );
    INIT_BLOCK_TYPE_INFO_( IR_BLOCK_TYPE_INVALID       , "block_invalid" );

#undef INIT_BLOCK_TYPE_INFO_
// ========================================================================


// ====================== OPERATIONS TYPE INFO ============================
#define INIT_OPERATION_INFO_(operation_type_, debug_name_)   \
    const static IrOperationTypeInfo struct_info_##operation_type_ = {operation_type_ ON_IR_DEBUG( , debug_name_)}

                        //    operation_type         debug_name
    INIT_OPERATION_INFO_( IR_OPERATION_TYPE_ADD     , "+"          );
    INIT_OPERATION_INFO_( IR_OPERATION_TYPE_SUB     , "-"          );
    INIT_OPERATION_INFO_( IR_OPERATION_TYPE_MUL     , "*"          );
    INIT_OPERATION_INFO_( IR_OPERATION_TYPE_DIV     , "/"          );
    INIT_OPERATION_INFO_( IR_OPERATION_TYPE_SQRT    , "sqrt"       );
    INIT_OPERATION_INFO_( IR_OPERATION_TYPE_POW     , "^"          );
    INIT_OPERATION_INFO_( IR_OPERATION_TYPE_EQ      , "=="         );
    INIT_OPERATION_INFO_( IR_OPERATION_TYPE_NEQ     , "!="         );
    INIT_OPERATION_INFO_( IR_OPERATION_TYPE_LESS    , "<"          );
    INIT_OPERATION_INFO_( IR_OPERATION_TYPE_LESSEQ  , "<="         );
    INIT_OPERATION_INFO_( IR_OPERATION_TYPE_GREAT   , ">"          );
    INIT_OPERATION_INFO_( IR_OPERATION_TYPE_GREATEQ , ">="         );
    INIT_OPERATION_INFO_( IR_OPERATION_TYPE_NONE    , "op_invalid" );

#undef INIT_BLOCK_TYPE_INFO_
// ========================================================================


const IrBlockTypeInfo *GetIrBlockTypeInfo(IrBlockType block_type)
{
    #define TYPE_INFO_CASE_(cmp_block_type_)  case cmp_block_type_: return &struct_info_##cmp_block_type_

    switch (block_type)
    {
        TYPE_INFO_CASE_( IR_BLOCK_TYPE_CALL_FUNCTION );
        TYPE_INFO_CASE_( IR_BLOCK_TYPE_FUNCTION_BODY );
        TYPE_INFO_CASE_( IR_BLOCK_TYPE_COND_JUMP     );
        TYPE_INFO_CASE_( IR_BLOCK_TYPE_NEG_COND_JUMP );
        TYPE_INFO_CASE_( IR_BLOCK_TYPE_ASSIGNMENT    );
        TYPE_INFO_CASE_( IR_BLOCK_TYPE_OPERATION     );
        TYPE_INFO_CASE_( IR_BLOCK_TYPE_RETURN        );
        TYPE_INFO_CASE_( IR_BLOCK_TYPE_LOCAL_LABEL   );
        TYPE_INFO_CASE_( IR_BLOCK_TYPE_SYSCALL       );
        TYPE_INFO_CASE_( IR_BLOCK_TYPE_INVALID       );

        default:
            lassert(NULL, "Unknown block type");
            return NULL;
    }

    #undef TYPE_INFO_CASE_
}

const IrOperationTypeInfo *GetIrOperationTypeInfo(IrOperationType operation_type)
{
    #define TYPE_INFO_CASE_(cmp_operation_type_)  case cmp_operation_type_: return &struct_info_##cmp_operation_type_

    switch (operation_type)
    {
        TYPE_INFO_CASE_( IR_OPERATION_TYPE_ADD               );
        TYPE_INFO_CASE_( IR_OPERATION_TYPE_SUB               );
        TYPE_INFO_CASE_( IR_OPERATION_TYPE_MUL               );
        TYPE_INFO_CASE_( IR_OPERATION_TYPE_DIV               );
        TYPE_INFO_CASE_( IR_OPERATION_TYPE_SQRT              );
        TYPE_INFO_CASE_( IR_OPERATION_TYPE_POW               );
        TYPE_INFO_CASE_( IR_OPERATION_TYPE_EQ                );
        TYPE_INFO_CASE_( IR_OPERATION_TYPE_NEQ               );
        TYPE_INFO_CASE_( IR_OPERATION_TYPE_LESS              );
        TYPE_INFO_CASE_( IR_OPERATION_TYPE_LESSEQ            );
        TYPE_INFO_CASE_( IR_OPERATION_TYPE_GREAT             );
        TYPE_INFO_CASE_( IR_OPERATION_TYPE_GREATEQ           );
        TYPE_INFO_CASE_( IR_OPERATION_TYPE_NONE              );

        default:
            lassert(NULL, "Unknown operation type");
            return NULL;
    }

    #undef TYPE_INFO_CASE_
}


IrFuncRes IrCtor(IR_struct *ir, const size_t start_capacity)
{
    lassert(ir, "");
    lassert(start_capacity > 0, "");

    ir->blocks = (IrBlock *) calloc(start_capacity, sizeof(IrBlock));
    
    if (ir->blocks == NULL)
        log(ERROR, "error in calloc");

    ir->capacity = start_capacity;
    ir->size     = 0;

    ir->proper_names_count = {};
    
    for (size_t i = 0; i < ir->capacity; i++)
    {
        SetPoisonOperationBlock(ir->blocks + i);
    }
    
    IR_VERIFY(ir, IR_FUNC_FAIL);

    return IR_FUNC_OK;
}

IrFuncRes IrDtor(IR_struct *ir)
{
    IR_VERIFY(ir, IR_FUNC_FAIL);

    free(ir->blocks);

    ir->capacity = 0;
    ir->size     = 0;

    return IR_FUNC_OK;
}

IrFuncRes IrRecalloc(IR_struct *ir, const size_t new_capacity)
{
    lassert(new_capacity >= ir->capacity, "invalid recalloc size");
    IR_VERIFY(ir, IR_FUNC_FAIL);

    ir->blocks = (IrBlock *) realloc(ir->blocks, new_capacity * sizeof(IrBlock));

    if (ir->blocks == NULL)
        log(ERROR, "error in recalloc");

    size_t prev_capacity = ir->capacity;
    ir->capacity = new_capacity;
    
    for (size_t i = prev_capacity; i < new_capacity; i++)
    {
        SetPoisonOperationBlock(ir->blocks + i);
    }

    IR_VERIFY(ir, IR_FUNC_FAIL);

    return IR_FUNC_OK;
}


IrBlock *IrNewBlock(IR_struct *ir, const IrBlockType block_type, const IrOperationType operation_type, 
                    const IrOperand ret_operand, const IrOperand operand_1, const IrOperand operand_2, const Label label)
{
    IR_VERIFY(ir, NULL);

    if (ir->size >= ir->capacity)
        IR_ERROR_HANDLE(IrRecalloc(ir, ir->capacity * 2));

    IrBlock *new_block = &ir->blocks[ir->size++];
    lassert(new_block);

    new_block->block_type_info = GetIrBlockTypeInfo(block_type);
    new_block->operation_info  = GetIrOperationTypeInfo(operation_type);

    new_block->operand_1   = operand_1;
    new_block->operand_2   = operand_2;
    new_block->ret_operand = ret_operand;

    new_block->label       = label;

    IR_VERIFY(ir, NULL);

    return new_block;
}

static void SetPoisonOperationBlock(IrBlock *operation_block)
{
    lassert(operation_block, "");

    // operation_block->block_type       = IR_BLOCK_TYPE_INVALID; 
    // operation_block->op_type          = IR_OPERATION_TYPE_NONE;

    operation_block->block_type_info  = GetIrBlockTypeInfo(IR_BLOCK_TYPE_INVALID);      // by default
    operation_block->operation_info   = GetIrOperationTypeInfo(IR_OPERATION_TYPE_NONE);

    operation_block->operand_1.type   = IR_OPERAND_TYPE_NONE;
    operation_block->operand_2.type   = IR_OPERAND_TYPE_NONE;
    operation_block->ret_operand.type = IR_OPERAND_TYPE_NONE;
}

// IrFuncRes NewIrByTreeNode(IR_struct *ir, Tree *code_tree, Node *cur_node)
// {
//     IR_VERIFY(ir, IR_FUNC_FAIL);
//     lassert(code_tree, "");

//     if (cur_node == NULL)
//         return IR_FUNC_OK;

//     if (cur_node->type == NODE_KEY_WORD)
//     {
//         // ...
//     }

//     if (cur_node->type == NODE_MATH_OP)
//     {
//         IrBlock *new_operation_block = IrNewBlock(ir);


//     }

//     IR_VERIFY(ir, IR_FUNC_FAIL);
// }



/*static*/ IrOperand NodeToIrOperand(IR_struct *ir, const Node *const cur_node)
{
    IR_VERIFY(ir, {});
    
    if (cur_node == NULL)
        return NONE_OPERAND;
    
    switch (cur_node->type)
    {
    case NODE_MATH_OP:
        return GetIrOperation(ir, cur_node); //  ret_op;
    
    case NODE_NUM:
        return GetIrNum(ir, cur_node);

    case NODE_VAR:
        return GetIrVar(ir, cur_node);

    case NODE_KEY_WORD:
        return GetIrKeyWord(ir, cur_node);

    case NODE_NEW_BLOCK:
        NodeToIrOperand(ir, cur_node->left);
        NodeToIrOperand(ir, cur_node->right);
        return NONE_OPERAND;

    case NODE_POISON_TYPE:
        log(ERROR, "node of poison type");
        return NONE_OPERAND;

    case NODE_VAR_OR_FUNC:
        log(ERROR, "undefined type of var or func '%s'", cur_node->val.prop_name->name);
        return NONE_OPERAND;

    case NODE_FUNC:
        log(ERROR, "wtf node_func?");
        return NONE_OPERAND;

    case NODE_MANAGER:
        log(ERROR, "wtf MANAGER??");
        return NONE_OPERAND;

    default:
        log(ERROR, "wtf, unknown type");
        return NONE_OPERAND;
    }    
        
    IR_VERIFY(ir, {});
}

static IrOperand GetIrKeyWord(IR_struct *ir, const Node *const key_word_node)
{
    switch (key_word_node->val.key_word->name)
    {
        case TREE_VAR_T_INDICATOR:
            // [[fallthrough]];
        case TREE_FUNC_T_INDICATOR:
            return NodeToIrOperand(ir, key_word_node->left);
        
        case TREE_INIT:
        {
            if (key_word_node->left->type == NODE_KEY_WORD && key_word_node->left->val.key_word->name == TREE_VAR_T_INDICATOR)
                return GetIrVarInit(ir, key_word_node);
            
            else if (key_word_node->left->type == NODE_KEY_WORD && key_word_node->left->val.key_word->name == TREE_FUNC_T_INDICATOR)
                return GetIrFuncInit(ir, key_word_node);

            // else
            log(ERROR, "invalid indicator node type");
            return NONE_OPERAND;
        }

        case TREE_NEW_EXPR:
            NodeToIrOperand(ir, key_word_node->left);
            NodeToIrOperand(ir, key_word_node->right);
            return NONE_OPERAND;

        case TREE_ASSIGN:
            return GetIrAssign(ir, key_word_node);
        
        case TREE_WHILE:
            return GetIrWhile(ir, key_word_node);

        case TREE_IF:
            return GetIrIf(ir, key_word_node);

        case TREE_NEW_FUNC:
            GetIrFuncInit(ir, key_word_node->left);
            NodeToIrOperand(ir, key_word_node->right);
            return NONE_OPERAND;

        case TREE_RETURN:
            return GetFuncReturn(ir, key_word_node);

        case TREE_FUNC_CALL:
            return GetCallFunc(ir, key_word_node);

        case TREE_COMMA:
            return GetCallParam(ir, key_word_node);

        case TREE_SPU_IN:
            return GetSysCallScan(ir, key_word_node);

        case TREE_SPU_OUT:
            return GetSysCallPrint(ir, key_word_node);

        default:
            return NONE_OPERAND;
    }
}


static IrOperand GetIrVarInit(IR_struct *ir, const Node *const var_init_node)
{
    IR_VERIFY(ir, NONE_OPERAND);
    lassert(var_init_node);
    lassert(var_init_node->type == NODE_KEY_WORD && var_init_node->val.key_word->name == TREE_INIT);

    Node *var_node      = var_init_node->left->left;
    Node *assigned_node = var_init_node->right;
    
    lassert(var_node->type == NODE_VAR);

    IrOperand dest_operand   = {.type = IR_OPERAND_TYPE_VAR, .val = {.var = var_node->val.prop_name->number}};
    IrOperand source_operand = NodeToIrOperand(ir, assigned_node);
    
    IrBlock *assign_block = NewAssignIrBlock(ir, dest_operand, source_operand);
    
    ON_IR_DEBUG(
    LoadOperandRealName(&assign_block->operand_1, var_node);
    MakeBlockComment(assign_block);
    )

    IR_VERIFY(ir, NONE_OPERAND);
    return NONE_OPERAND;
}


static IrOperand GetIrAssign(IR_struct *ir, const Node *const assign_node)
{
    IrOperand dest_operand   = NodeToIrOperand(ir, assign_node->left);
    IrOperand source_operand = NodeToIrOperand(ir, assign_node->right);

    NewAssignIrBlock(ir, dest_operand, source_operand);

    return NONE_OPERAND;
}


static IrOperand GetIrIf(IR_struct *ir, const Node *const if_node)
{
    const Node *condition_node  = if_node->left;
    const Node *if_body_node    = if_node->right;

    IrOperand condition_operand = NodeToIrOperand(ir, condition_node);
    IrBlock  *jz_to_end_block   = NewJzIrBlock  (ir, {}, condition_operand);    // test condition
                                                                                // jz _unknown_label_   (further - end_cycle_label)

    NodeToIrOperand(ir, if_body_node);                                          // { if_body }
                           
    IrBlock   *end_label_block  = NewLocalLabelBlock(ir);                       // end_body_label:
    IrOperand end_label_operand = {.type = IR_OPERAND_TYPE_LOCAL_LABEL, .val = {.label = end_label_block->label}};
    jz_to_end_block->operand_1  = end_label_operand;                            // _unknown_label_ -> end_cycle_label

    return NONE_OPERAND;
}


static IrOperand GetIrWhile(IR_struct *ir, const Node *const while_node)
{
    const Node *condition_node  = while_node->left;
    const Node *while_body_node = while_node->right;

    IrBlock   *start_label_block  = NewLocalLabelBlock(ir);                     // start_cycle_label:
    IrOperand start_label_operand = {.type = IR_OPERAND_TYPE_LOCAL_LABEL, .val = {.label = start_label_block->label}};

    IrOperand condition_operand = NodeToIrOperand(ir, condition_node);
    IrBlock  *jz_to_end_block   = NewJzIrBlock  (ir, {}, condition_operand);    // test condition
                                                                                // jz _unknown_label_   (further - end_cycle_label)

    NodeToIrOperand(ir, while_body_node);                                       // { cycle_body }

    IrBlock *jmp_to_start_block = NewJumpIrBlock(ir, start_label_operand);

    IrBlock   *end_label_block  = NewLocalLabelBlock(ir);                       // end_cycle_label:
    IrOperand end_label_operand = {.type = IR_OPERAND_TYPE_LOCAL_LABEL, .val = {.label = end_label_block->label}};
    jz_to_end_block->operand_1  = end_label_operand;                            // _unknown_label_ -> end_cycle_label

    ON_IR_DEBUG(
    strncat(jmp_to_start_block->comment, "cycle_end",   COMMENT_LEN);
    strncat(jz_to_end_block->comment,    "cycle_start", OPERAND_NAME_LEN);
    )

    return NONE_OPERAND;
}


static IrOperand GetIrOperation(IR_struct *ir, const Node *const math_op_node)
{
    IrOperand operand_1 = NodeToIrOperand(ir, math_op_node->left);
    IrOperand operand_2 = NodeToIrOperand(ir, math_op_node->right);
    
    IrOperationType operation_type = TreeToIrMathOp(math_op_node->val.math_op->num);
    
    // size_t new_tmp_num = ir->proper_names_count.tmp_count++;
    // IrOperand ret_op   = {.type = IR_OPERAND_TYPE_TMP, .val = {.tmp = new_tmp_num}};
    
    IrOperand ret_op = IrNewTmpOperand(ir);

    IrNewBlock(ir, IR_BLOCK_TYPE_OPERATION, operation_type, ret_op, operand_1, operand_2, {});
    
    return ret_op;
}


static IrOperand GetIrVar(IR_struct *ir, const Node *const var_node)        // tmp = var
{
    return {.type = IR_OPERAND_TYPE_VAR, .val = {.var = var_node->val.prop_name->number}};
}


static IrOperand IrNewTmpOperand(IR_struct *ir)
{
    size_t new_tmp_num = ir->proper_names_count.tmp_count++;
    return {.type = IR_OPERAND_TYPE_TMP, .val = {.tmp = new_tmp_num}};
}


static IrOperand GetIrNum(IR_struct *ir, const Node *const num_node)          // tmp = num
{
    return {.type = IR_OPERAND_TYPE_NUM, .val = {.num = num_node->val.num}};
}


static IrOperand GetIrFuncInit(IR_struct *ir, const Node *const func_init_node)
{
    IR_VERIFY(ir, NONE_OPERAND);

    Node *func_t_indictor_node = func_init_node->left;
    Node *func_node = func_t_indictor_node->left; 
    lassert(func_node && func_node->type == NODE_FUNC);

    size_t func_num = func_node->val.prop_name->number;
    
    size_t arg_cnt = GetFuncArgsNum(func_t_indictor_node);

    if (strcmp(func_node->val.prop_name->name, MAIN_FUNC_NAME) == 0)
        func_num = MAIN_FUNC_NUM;

    Label func_label  = {.func = {.num = func_num, .arg_cnt = arg_cnt}};
    
    IrBlock *new_func_block = IrNewBlock(ir, IR_BLOCK_TYPE_FUNCTION_BODY, IR_OPERATION_TYPE_NONE, NONE_OPERAND, NONE_OPERAND, NONE_OPERAND, func_label);
    
    ON_IR_DEBUG(
    AddBlockComment(new_func_block, "int %s(%lu args)", func_node->val.prop_name->name, arg_cnt);
    )

    NodeToIrOperand(ir, func_init_node->right);

    return NONE_OPERAND;
}


static IrOperand GetFuncReturn(IR_struct *ir, const Node *const ret_node)
{
    IR_VERIFY(ir, NONE_OPERAND);
    lassert(ret_node && ret_node->type == NODE_KEY_WORD && ret_node->val.key_word->name == TREE_RETURN);

    IrOperand ret_operand = NodeToIrOperand(ir, ret_node->left);

    IrNewBlock(ir, IR_BLOCK_TYPE_RETURN, IR_OPERATION_TYPE_NONE, ret_operand, NONE_OPERAND, NONE_OPERAND, {});

    return NONE_OPERAND;
}


static IrOperand GetCallParam(IR_struct *ir, const Node *const param_node)
{
    IR_VERIFY(ir, NONE_OPERAND);
    lassert(param_node && param_node->type == NODE_KEY_WORD && param_node->val.key_word->name == TREE_COMMA);

    Node *arg_node = param_node->left;

    IrOperand arg_source_operand = NodeToIrOperand(ir, arg_node);
    IrOperand arg_dest_operand   = {.type = IR_OPERAND_TYPE_ARG, .val = ir->proper_names_count.arg_count++};

    NewAssignIrBlock(ir, arg_dest_operand, arg_source_operand);

    NodeToIrOperand(ir, param_node->right);

    return NONE_OPERAND;
}


static IrOperand GetCallFunc(IR_struct *ir, const Node *const call_node)
{
    IR_VERIFY(ir, NONE_OPERAND);
    lassert(call_node && call_node->type == NODE_KEY_WORD && call_node->val.key_word->name == TREE_FUNC_CALL);

    Node *func_t_indicator = call_node->left;
    Node *func_node        = func_t_indicator->left;
    Node *params_node      = func_t_indicator->right;

    size_t prev_arg_cnt = ir->proper_names_count.arg_count;
    ir->proper_names_count.arg_count = 0;
    GetCallParam(ir, params_node);
    ir->proper_names_count.arg_count = prev_arg_cnt;

    size_t arg_cnt = GetFuncArgsNum(func_t_indicator);
    Label func_label = {.func = {.num = func_node->val.prop_name->number, .arg_cnt = arg_cnt}};
    
    IrOperand func_label_operand = {.type = IR_OPERAND_TYPE_FUNC_LABEL, .val = {.label = func_label}};
    
    IrOperand ret_tmp_operand = IrNewTmpOperand(ir);

    IrBlock *call_func_block = IrNewBlock(ir, IR_BLOCK_TYPE_CALL_FUNCTION, IR_OPERATION_TYPE_NONE, ret_tmp_operand, func_label_operand, NONE_OPERAND, {});

    ON_IR_DEBUG(
    AddBlockComment(call_func_block, "call %s(%lu args)", func_node->val.prop_name->name, arg_cnt);
    )

    return ret_tmp_operand;
}


static IrOperand GetSysCallScan(IR_struct *ir, const Node *const sys_func_node)
{
    IR_VERIFY(ir, NONE_OPERAND);
    lassert(sys_func_node->type == NODE_KEY_WORD && sys_func_node->val.key_word->name == TREE_SPU_IN);

    Label scan_label = {.sys = sys_func_node->val.key_word->real_symbol};
    IrOperand res_tmp_operand = IrNewTmpOperand(ir);

    IrNewBlock(ir, IR_BLOCK_TYPE_SYSCALL, IR_OPERATION_TYPE_NONE, res_tmp_operand, NONE_OPERAND, NONE_OPERAND, scan_label);

    return res_tmp_operand;
}


static IrOperand GetSysCallPrint(IR_struct *ir, const Node *const sys_func_node)
{
    IR_VERIFY(ir, NONE_OPERAND);
    lassert(sys_func_node->type == NODE_KEY_WORD && sys_func_node->val.key_word->name == TREE_SPU_OUT);

    Label print_label = {.sys = sys_func_node->val.key_word->real_symbol};

    Node *print_arg = sys_func_node->left;
    IrOperand arg_operand = NodeToIrOperand(ir, print_arg);

    IrNewBlock(ir, IR_BLOCK_TYPE_SYSCALL, IR_OPERATION_TYPE_NONE, NONE_OPERAND, arg_operand, NONE_OPERAND, print_label);

    return NONE_OPERAND;
}


static IrBlock *NewOperationBlock(IR_struct *ir, IrOperationType operation_type, IrOperand operand1, IrOperand operand2)
{
    IrOperand ret_operand = IrNewTmpOperand(ir);
    return IrNewBlock(ir, IR_BLOCK_TYPE_ASSIGNMENT, operation_type, ret_operand, operand1, operand2, {});
}


static IrBlock *NewAssignIrBlock(IR_struct *ir, IrOperand dest, IrOperand source)
{
    return IrNewBlock(ir, IR_BLOCK_TYPE_ASSIGNMENT, IR_OPERATION_TYPE_NONE, NONE_OPERAND, dest, source, {});
}


static IrBlock *NewCndJumpIrBlock(IR_struct *ir, IrOperand jmp_label, IrOperand condition)
{
    return IrNewBlock(ir, IR_BLOCK_TYPE_COND_JUMP, IR_OPERATION_TYPE_NONE, NONE_OPERAND, jmp_label, condition, {});
}


static IrBlock *NewJzIrBlock(IR_struct *ir, IrOperand jmp_label, IrOperand condition)
{
    return IrNewBlock(ir, IR_BLOCK_TYPE_NEG_COND_JUMP, IR_OPERATION_TYPE_NONE, NONE_OPERAND, jmp_label, condition, {});
}


static IrBlock *NewJumpIrBlock(IR_struct *ir, IrOperand jmp_label)
{
    IrOperand num_one_operand = {.type = IR_OPERAND_TYPE_NUM, .val = {.num = 1}};

    return IrNewBlock(ir, IR_BLOCK_TYPE_COND_JUMP, IR_OPERATION_TYPE_NONE, NONE_OPERAND, jmp_label, num_one_operand, {});   
}


static IrBlock *NewLocalLabelBlock(IR_struct *ir)
{
    Label local_label = {.local = ir->proper_names_count.local_label_count++};
    IrBlock *new_label_block = IrNewBlock(ir, IR_BLOCK_TYPE_LOCAL_LABEL, IR_OPERATION_TYPE_NONE, NONE_OPERAND, NONE_OPERAND, NONE_OPERAND, local_label);

    return new_label_block;
}




static IrOperationType TreeToIrMathOp(MathOperation_enum cur_tree_math_op)
{
    #define SWITCH_MATH_OP(tree_math_op, ir_math_op) case tree_math_op: return ir_math_op

    switch (cur_tree_math_op)
    {
        SWITCH_MATH_OP (TREE_BOOL_EQ         , IR_OPERATION_TYPE_EQ      );
        SWITCH_MATH_OP (TREE_BOOL_NEQ        , IR_OPERATION_TYPE_NEQ     );
        SWITCH_MATH_OP (TREE_BOOL_LOWER      , IR_OPERATION_TYPE_LESS    );
        SWITCH_MATH_OP (TREE_BOOL_GREATER    , IR_OPERATION_TYPE_GREAT   );
        SWITCH_MATH_OP (TREE_BOOL_LOWER_EQ   , IR_OPERATION_TYPE_LESSEQ  );
        SWITCH_MATH_OP (TREE_BOOL_GREATER_EQ , IR_OPERATION_TYPE_GREATEQ );
        SWITCH_MATH_OP (TREE_ADD             , IR_OPERATION_TYPE_ADD     );
        SWITCH_MATH_OP (TREE_SUB             , IR_OPERATION_TYPE_SUB     );
        SWITCH_MATH_OP (TREE_MUL             , IR_OPERATION_TYPE_MUL     );
        SWITCH_MATH_OP (TREE_DIV             , IR_OPERATION_TYPE_DIV     );
        SWITCH_MATH_OP (TREE_SQRT            , IR_OPERATION_TYPE_SQRT    );
        SWITCH_MATH_OP (TREE_DEG             , IR_OPERATION_TYPE_POW     );
        
        case TREE_LN   :
        case TREE_LOG2 :
        case TREE_SIN  :
        case TREE_COS  :
        case TREE_TAN  :
        default        :
            return IR_OPERATION_TYPE_NONE;
    }

    #undef SWITCH_MATH_OP
}


