#ifndef SEMANTIC_ANAL_H
#define SEMANTIC_ANAL_H

#include <stdio.h>

#include "tree_lib.h"

// const size_t ERROR_NAME_LEN = 100;

void SemanticAnal(Tree *tree);
void CheckProperNamesValidity(Tree *tree, Node *cur_node);

// void SyntaxError(Tree *tree, Node *cur_node, const char *error, const char *file, int line, const char *func);

// #define SYNTAX_ERROR(tree, cur_token, error)  SyntaxError(tree, cur_token, error, __FILE__, __LINE__, __func__)


#endif