#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "tree.h"
#include "tree_graph.h"
#include "operations.h"

void DrawGraph(Tree *tree, char *dest_picture_path)
{
    assert(tree);
    assert(dest_picture_path);

    FILE *dot_file = fopen(TMP_DOTFILE_NAME, "w");

    fprintf(dot_file, "digraph G{               \n"
                        "bgcolor = \"%s\";      \n" 
                        "edge [color = \"%s\"]; \n", BACKGROUND_COLOR, EDGE_COLOR);

    InitNodesInDot(tree, dot_file);
    MakeLinksInDot(tree, dot_file);

    fprintf(dot_file, "} \n");

    fclose(dot_file);

    MakeGraphPicture(TMP_DOTFILE_NAME, dest_picture_path);
}

void InitNodesInDot(Tree *tree, FILE *dot_file)
{
    assert(tree);
    assert(dot_file);
    assert(tree->node_ptrs);

    for (size_t node_counter = 0; node_counter < tree->size; node_counter++)
    {
        Node *cur_node = tree->node_ptrs[node_counter];

        if (cur_node == NULL)
            continue;

        char *node_val_str = NodeValToStr(cur_node);

        for (size_t type_counter = 0; type_counter < NODE_TYPES_COUNT; type_counter++)
        {
            if (cur_node->type == GraphViewInfo[type_counter].type)
            {
                fprintf(dot_file, "%s%p [shape = \"%s\", style = filled, fillcolor = \"%s\", label = \"%s\"]\n",
                    NODE_NAME_PREFIX, cur_node, GraphViewInfo[type_counter].node_shape, GraphViewInfo[type_counter].node_color, node_val_str);

                continue;
            }
        }
    }
}

void MakeLinksInDot(Tree *tree, FILE *dot_file)
{
    assert(tree);
    assert(dot_file);

    for (size_t i = 0; i < tree->size; i++)
    {
        Node *cur_node  = tree->node_ptrs[i];
        
        if (cur_node == NULL)
            continue;

        Node *left_son  = cur_node->left;
        Node *right_son = cur_node->right;

        if (left_son  != NULL)
            fprintf(dot_file, "%s%p -> %s%p\n", NODE_NAME_PREFIX, cur_node,  NODE_NAME_PREFIX, left_son);

        if (right_son != NULL)
            fprintf(dot_file, "%s%p -> %s%p\n", NODE_NAME_PREFIX, cur_node, NODE_NAME_PREFIX, right_son);
    }
}

void MakeGraphPicture(const char *dotfile_path, const char *picture_path)
{
    assert(dotfile_path);
    assert(picture_path);

    char cmd_command[CMD_COMMAND_LEN] = {};

    sprintf(cmd_command, "dot %s -T png -o %s\n", dotfile_path, picture_path);
    system(cmd_command);
}