#include <stdio.h>
#include <stdlib.h>

#include "lang.h"
#include "debug.h"
#include "tree.h"


//=========================================================================

int treeCtor(tree_t* tree)
{
    CHECK(tree !=  NULL, ERR_TREE_NULL_PTR);

    tree->root = NULL;
    tree->status = TREE_SUCCESS;

    return TREE_SUCCESS;
}

//=========================================================================

node_t* createNodeOp(int val, node_t* left, node_t* right)
{
    node_t* node = (node_t*) calloc(1, sizeof(node_t));
    CHECK(node !=  NULL, NULL);

    node->type  = OP;
    node->data.opValue = val;

    node->left  = left;
    node->right = right;

    return node; 
}

//=========================================================================

node_t* createNodeKey(int val, node_t* left, node_t* right)
{
    node_t* node = (node_t*) calloc(1, sizeof(node_t));
    CHECK(node !=  NULL, NULL);

    node->type  = KEY;
    node->data.keyValue = val;

    node->left  = left;
    node->right = right;

    return node; 
}

//=========================================================================

node_t* copyNode(node_t* prev_node)
{
    node_t* node = (node_t*) calloc(1, sizeof(node_t));
    CHECK(node !=  NULL, NULL);

    switch(prev_node->type)
    {
    case NUM:
        node->type = NUM;
        node->data.dblValue = prev_node->data.dblValue;        
        break;
    
    case VAR:
        node->type = VAR;
        node->data.varValue = prev_node->data.varValue;
        break;

    case OP:
        node->type = OP;
        node->data.opValue = prev_node->data.opValue;
        break;

    default:
        break;
    }

    if(prev_node->left != NULL)
    {
        node_t* left = copyNode(prev_node->left);
        node->left = left;
    }
    else
    {
        node->left  = NULL;
    }
    
    if(prev_node->right != NULL)
    {
        node_t* right = copyNode(prev_node->right);
        node->right = right;
        
        return node;
    }
    node->right = NULL;

    return node;     
}

//=========================================================================

node_t* createNum(int val)
{
    node_t* node = (node_t*) calloc(1, sizeof(node_t));
    CHECK(node !=  NULL, NULL);

    node->type  = NUM;
    node->data.dblValue = val;

    node->left  = NULL;
    node->right = NULL;
 
    return node;
}

//=========================================================================

node_t* createVar(const char* val)
{
    node_t* node = (node_t*) calloc(1, sizeof(node_t));
    CHECK(node !=  NULL, NULL);

    node->type  = VAR;
    node->data.varValue = val;

    node->left  = NULL;
    node->right = NULL;
 
    return node;
}

//=========================================================================

node_t* createOp(int opValue)
{
    node_t* node = (node_t*) calloc(1, sizeof(node_t));
    CHECK(node !=  NULL, NULL);

    node->type  = OP;
    node->data.opValue = opValue;

    node->left  = NULL;
    node->right = NULL;
 
    return node;
}

//=========================================================================

node_t* createKey(int keyValue)
{
    node_t* node = (node_t*) calloc(1, sizeof(node_t));
    CHECK(node !=  NULL, NULL);

    node->type  = KEY;
    node->data.keyValue = keyValue;

    node->left  = NULL;
    node->right = NULL;
 
    return node;
}

//=========================================================================

int treeDtor(tree_t* tree)
{
    CHECK(tree !=  NULL, ERR_TREE_NULL_PTR);

    treeNodeDtor(tree->root);
    tree->root = NULL;
    tree->status = TREE_ERROR;

    return TREE_SUCCESS;
}

//=========================================================================

void treeNodeDtor(node_t* node)
{
    if(node == NULL)
    {
        return;
    }

    treeNodeDtor(node->left);
    treeNodeDtor(node->right);
    free(node);
}

//=========================================================================
