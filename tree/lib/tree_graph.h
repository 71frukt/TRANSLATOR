#ifndef DIFF_GRAPH_H
#define DIFF_GRAPH_H

#include "tree.h"

#define TMP_DOTFILE_NAME     "tmp_doc_code.doc"
#define GRAPH_NAME_PREFIX    "graph_"
#define NODE_NAME_PREFIX     "node_"

#define BACKGROUND_COLOR     "darkslategray"
#define EDGE_COLOR           "aqua"

const int GRAPH_IMG_WIDTH = 20;     // (%)
const int CMD_COMMAND_LEN = 100;

void  DrawGraph        (Tree *tree, char *dest_picture_path);
void  InitNodesInDot   (Tree *tree, FILE *dot_file);
void  MakeLinksInDot   (Tree *tree, FILE *dot_file);
void  MakeGraphPicture (const char *dotfile_path, const char *picture_path);

struct NodeGraphInfo
{
    const NodeType    type;
    const char *const node_color;
    const char *const node_shape;
};

const NodeGraphInfo GraphViewInfo[NODE_TYPES_COUNT] = 
{
    { NUM,         "skyblue",       "oval"    },
    { VAR,         "aquamarine",    "square"  },
    { FUNC,        "darkturquoise", "square"  },
    { VAR_OR_FUNC, "red",           "square"  },
    { MATH_OP,     "seagreen1",     "record"  },
    { KEY_WORD,    "tan",           "egg"     },
    { MANAGER,     "grey",          "oval"    },
    { NEW_BLOCK,   "yellow",        "hexagon" },
    { POISON_TYPE, "red",           "hexagon" }
};

#endif