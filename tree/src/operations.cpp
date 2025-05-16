#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include "operations.h"
#include "tree_debug.h"
#include "asm_operations.h"

extern FILE *LogFile;

const MathOperation *GetOperationBySymbol(char *sym, SymbolMode mode) // TODO: move asm from here
{
    assert(sym);

    for (size_t i = 0; i < MATH_OPERATIONS_NUM; i++)
    {
        const char *compare_sym = (mode == MY_CODE_MODE ? MathOperations[i].my_symbol : MathOperations[i].real_symbol);

        if (strcmp(sym, compare_sym) == 0)
            return &MathOperations[i];
    }

    return NULL;
}

const KeyWord *GetKeyWordBySymbol(char *sym, SymbolMode mode)
{
    assert(sym);

    for (size_t i = 0; i < KEY_WORDS_NUM; i++)
    {
        const char *compare_sym = (mode == MY_CODE_MODE ? KeyWords[i].my_symbol : KeyWords[i].real_symbol);

        if (compare_sym == NULL)
            continue;

        if (strcmp(sym, compare_sym) == 0)
            return &KeyWords[i];
    }

    return NULL;
}

const ManageElem *GetManageElemBySymbol(char *sym, SymbolMode mode)
{
    assert(sym);

    for (size_t i = 0; i < MANAGE_ELEMS_NUM; i++)
    {
        const char *compare_sym = (mode == MY_CODE_MODE ? Managers[i].my_symbol : Managers[i].real_symbol);

        if (strcmp(sym, compare_sym) == 0)
            return &Managers[i];
    }

    return NULL;
}

TreeElem_t IsEqual(Node *arg1, Node *arg2)
{
    return arg1->val.num == arg2->val.num;
}

TreeElem_t IsNotEqual(Node *arg1, Node *arg2)
{
    return arg1->val.num != arg2->val.num;
}

TreeElem_t IsBelow(Node *arg1, Node *arg2)
{
    return arg1->val.num < arg2->val.num;
}

TreeElem_t IsAbove(Node *arg1, Node *arg2)
{
    return arg1->val.num > arg2->val.num;
}

TreeElem_t IsBelowEq(Node *arg1, Node *arg2)
{
    return arg1->val.num <= arg2->val.num;
}

TreeElem_t IsAboveEq(Node *arg1, Node *arg2)
{
    return arg1->val.num >= arg2->val.num;
}

TreeElem_t Add(Node *arg1, Node *arg2)
{
    return arg1->val.num + arg2->val.num;
}

TreeElem_t Sub(Node *arg1, Node *arg2)
{
    return arg1->val.num - arg2->val.num;
}

TreeElem_t Mul(Node *arg1, Node *arg2)
{
    return arg1->val.num * arg2->val.num;
}

TreeElem_t Div(Node *arg1, Node *arg2)
{
    return arg1->val.num / arg2->val.num;
}

TreeElem_t Sqrt(Node *arg1, Node *arg2)
{
    return (TreeElem_t) pow(arg1->val.num, 0.5f);
}

TreeElem_t Deg(Node *arg1, Node *arg2)
{
    return (TreeElem_t) pow(arg1->val.num, arg2->val.num);
}

TreeElem_t Sin(Node *arg1, Node *arg2)
{
    return (TreeElem_t) sin(arg1->val.num);
}

TreeElem_t Cos(Node *arg1, Node *arg2)
{
    return (TreeElem_t) cos(arg1->val.num);
}

TreeElem_t Tan(Node *arg1, Node *arg2)
{
    return (TreeElem_t) tan(arg1->val.num);
}

TreeElem_t Ln(Node *arg1, Node *arg2)
{
    return (TreeElem_t) log(arg1->val.num);
}

TreeElem_t Log(Node *arg1, Node *arg2)
{
    return (TreeElem_t) (log(arg2->val.num) / log(arg1->val.num));
}
