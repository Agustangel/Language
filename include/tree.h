#pragma once

#include <stdio.h>
#include <stdint.h>


//=========================================================================

enum tree_codes
{
    TREE_ERROR                = -1,
    TREE_SUCCESS              =  0,
    ERR_TREE_NULL_PTR         =  1,
    ERR_TREE_OUT_MEMORY       =  2,
    ERR_TREE_BAD_SIZE         =  3,
    ERR_TREE_BAD_POSITION     =  4,
    ERR_TREE_BAD_STRING       =  5,
    ERR_TREE_BAD_FILE         =  6
};

enum node_type
{
    NUM = 1,
    VAR = 2,
    OP  = 3,
    KEY = 4
};

enum node_op
{
    OP_ERROR   = -1,
    OP_ADD     =  1,
    OP_SUB     =  2,
    OP_MUL     =  3,
    OP_DIV     =  4,
    OP_POW     =  5,
    OP_SIN     =  6,
    OP_COS     =  7,
    OP_EXP     =  8,
    OP_LN      =  9,
    OP_OPENBRT =  10,
    OP_CLOSBRT =  11,
    OP_CONNECT =  12,
    OP_EQUAL   =  13,
    OP_UNEQUAL =  14,
    OP_MORE    =  15,
    OP_LESS    =  16,
    OP_COMMA   =  17,
    OP_NOPARAM =  18,
    OP_SQRT    =  19,
    OP_AND     =  20,
    OP_OR      =  21
};

enum key_word
{
    KEY_IF     = 30,
    KEY_ELSE   = 31,
    KEY_WHILE  = 32,
    KEY_ASSIGN = 33,
    KEY_START  = 34,
    KEY_FUNC   = 35,
    KEY_RET    = 36,
    KEY_DEC    = 37
};

//=========================================================================

typedef uint_fast16_t treeStatus_t;
typedef struct node_t node_t;
typedef char* name_t;

typedef struct treeData_t
{
    int           intValue;
    char*   varValue;
    enum node_op  opValue;
    enum key_word keyValue;

} treeData_t;

typedef struct node_t
{
    enum node_type type;
    treeData_t     data;
    name_t  name;

    node_t* left;
    node_t* right;

} node_t;

typedef struct tree_t
{
    node_t* root;
    treeStatus_t status;
    name_t name_equation;

} tree_t;

//====================================================================================================================

#define MAKE_FRMT_MSG(sign) fprintf(dot_out, "\n\t\t\""#sign"_%p\"[shape = \"ellipse\", label = \""#sign"\", \
                                    color=\"#900000\", style=\"filled\", fillcolor = \"#D0FDFF\"];\n", node)
                                    

#define MAKE_CON_NUM(sign)  fprintf(dot_out, "\t\t\""#sign"_%p\"->\"%d_%p\";\n", node_prev, node->data.intValue, node)
#define MAKE_CON_VAR(sign)  fprintf(dot_out, "\t\t\""#sign"_%p\"->\"%c_%p\";\n", node_prev, *node->data.varValue, node)
#define MAKE_CON_OPB(sign)  fprintf(dot_out, "\t\t\""#sign"_%p\"->", node_prev)
#define MAKE_CON_OPE(sign)  fprintf(dot_out, "\""#sign"_%p\";\n", node)
#define MAKE_CON_KEYB(sign) fprintf(dot_out, "\t\t\""#sign"_%p\"->", node_prev)
#define MAKE_CON_KEYE(sign) fprintf(dot_out, "\""#sign"_%p\";\n", node)

//====================================================================================================================

int treeCtor(tree_t* tree, name_t name_program);
node_t* createNodeOp(int val, node_t* left, node_t* right);
node_t* createNodeKey(int val, node_t* left, node_t* right);
node_t* copyNode(node_t* prev_node);
node_t* createNum(int val);
node_t* createVar(char* val);
node_t* createOp(int opValue);
node_t* createKey(int keyValue);
int treeDtor(tree_t* tree);
void treeNodeDtor(node_t* node);
int dumpGraphTree(tree_t* tree);
int dumpGraphNode(node_t* node, FILE* dot_out);
int fprintfConnection(node_t* node_prev, node_t* node, int operation, FILE* dot_out);
name_t getNameFile(name_t file);
