#ifdef TREE_DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "tree.h"
#include "tree_debug.h"
#include "tree_graph.h"

extern FILE *OutputFile;

FILE *TreeLogFile = OpenTreeLogFile();

FILE *OpenTreeLogFile()
{
    char logfile_path[PATH_NAME_LEN] = {};
    GetFilePath(TREE_LOGFILE_NAME, LOGS_FOLDER_NAME, logfile_path);
// fprintf(stderr, "logfile_path = '%s'\n", logfile_path);
    FILE *logfile = fopen(logfile_path, "w");
    if (logfile == NULL)
        fprintf(stderr, "error opening logfile in OpenTreeLogFile()\n");

    setvbuf(logfile, NULL, _IONBF, 0);

    fprintf(logfile, "<html>                                                                                                            \n"
                            "\t<head>                                                                                                   \n"
                            "\t<title>Tree Logs</title>                                                                                 \n"
                            "\t<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/4.5.0/css/bootstrap.min.css\"> \n"
                            "\t</head>                                                                                                  \n"
                            "\t<body>                                                                                                   \n"
                            "\t<title>Tree Logs</title>                                                                                 \n"
                            "\t<div class=\"jumbotron text-center\">                                                                    \n"
                            "\t\t<h1>List logs</h1>                                                                                     \n"
                            "\t</div>                                                                                                   \n"
                            "\t<pre>                                                                                                    \n"
                        );

    atexit(CloseTreeLogFile);

    return (logfile);
}

void CloseTreeLogFile()
{
    fprintf(TreeLogFile,    "\t\t</pre>     \n"
                        "\t</body       \n"
                        "</html>");
    fclose(TreeLogFile);
}

char *GetFilePath(const char *name, const char *folder, char *path)
{
    sprintf(path, "%s/%s", folder, name);
    return path;
}

void TreeDump(Tree *tree, const char *file, int line, const char *func)
{
    assert(tree);
    assert(file);
    assert(func);

    static size_t drawn_graphs_num = 0;

    fprintf(TreeLogFile, "   Tree '%s' TREE_DUMP called from %s:%d  (%s)\n  {\n", tree->name, file, line, func);
    char picture_name[PATH_NAME_LEN] = {};
    sprintf(picture_name, "%s/%s/%s%lu.png", GRAPH_FOLDER_NAME, tree->name, GRAPH_NAME_PREFIX, drawn_graphs_num);

    DrawGraph(tree, picture_name);
    // fprintf(stderr, "<img src = %s/%s/%s%lu.png width = \"%d%%\" style=\"margin-left: 3%%\">\n", GRAPH_FOLDER_NAME, tree->name, GRAPH_NAME_PREFIX, drawn_graphs_num, GRAPH_IMG_WIDTH);
    // fprintf(TreeLogFile, "<img src = %s/%s/%s%lu.png width = \"%d%%\" style=\"margin-left: 3%%\">\n", GRAPH_FOLDER_NAME, tree->name, GRAPH_NAME_PREFIX, drawn_graphs_num, GRAPH_IMG_WIDTH);

    fprintf(TreeLogFile, "\n  }\n\n");

    drawn_graphs_num++;
}

#endif