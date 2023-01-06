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
};

enum key_word
{
    KEY_IF     = 1,
    KEY_ELSE   = 2,
    KEY_WHILE  = 3,
    KEY_ASSIGN = 4,
};

//=========================================================================

typedef uint_fast16_t treeStatus_t;
typedef struct node_t node_t;

typedef struct treeData_t
{
    double        dblValue;
    const char*   varValue;
    enum node_op  opValue;
    enum key_word keyValue;

} treeData_t;

typedef struct node_t
{
    enum node_type type;
    treeData_t     data;

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
node_t* createVar(const char* val);
node_t* createOp(int opValue);
int treeDtor(tree_t* tree);
void treeNodeDtor(node_t* node);
