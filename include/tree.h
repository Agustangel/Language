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
    OP_MORE    =  14,
    OP_LESS    =  15,
    OP_COMMA   =  16,
    OP_NOPARAM =  17,
    OP_SQRT    =  18,
    OP_AND     =  19,
    OP_OR      =  20
};

enum key_word
{
    KEY_IF     = 30,
    KEY_ELSE   = 31,
    KEY_WHILE  = 32,
    KEY_ASSIGN = 33,
    KEY_START  = 34,
    KEY_FUNC   = 35,
    KEY_RET    = 36
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

} tree_t;

//=========================================================================

int treeCtor(tree_t* tree);
node_t* createNodeOp(int val, node_t* left, node_t* right);
node_t* createNodeKey(int val, node_t* left, node_t* right);
node_t* copyNode(node_t* prev_node);
node_t* createNum(int val);
node_t* createVar(char* val);
node_t* createOp(int opValue);
int treeDtor(tree_t* tree);
void treeNodeDtor(node_t* node);
int dumpGraphTree(tree_t* tree);
int dumpGraphNode(node_t* node, FILE* dot_out);
int fprintfConnection(node_t* node_prev, node_t* node, int operation, FILE* dot_out);
