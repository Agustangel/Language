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
    CHECK(left  != NULL, NULL);
    CHECK(right != NULL, NULL);

    node_t* node = (node_t*) calloc(1, sizeof(node_t));
    CHECK(node !=  NULL, NULL);

    node->type  = OP;
    node->data.opValue = val;
    node->name = NAME_POISON;

    node->left  = left;
    node->right = right;

    return node; 
}

//=========================================================================

node_t* createNodeKey(int val, node_t* left, node_t* right)
{
    CHECK(left  != NULL, NULL);
    CHECK(right != NULL, NULL);
    
    node_t* node = (node_t*) calloc(1, sizeof(node_t));
    CHECK(node !=  NULL, NULL);

    node->type  = KEY;
    node->data.keyValue = val;
    node->name = NAME_POISON;

    node->left  = left;
    node->right = right;

    return node; 
}

//=========================================================================

node_t* copyNode(node_t* prev_node)
{
    CHECK(prev_node != NULL, NULL);

    node_t* node = (node_t*) calloc(1, sizeof(node_t));
    CHECK(node !=  NULL, NULL);

    node->name = prev_node->name;
    switch(prev_node->type)
    {
    case NUM:
        node->type = NUM;
        node->data.intValue = prev_node->data.intValue;        
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
    node->data.intValue = val;
    node->name = NAME_POISON;

    node->left  = NULL;
    node->right = NULL;

    return node;
}

//=========================================================================

node_t* createVar(char* val)
{
    CHECK(val != NULL, NULL);

    node_t* node = (node_t*) calloc(1, sizeof(node_t));
    CHECK(node !=  NULL, NULL);

    node->type  = VAR;
    node->data.varValue = val;
    node->name = NAME_POISON;    

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
    node->name = NAME_POISON;

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
    node->name = NAME_POISON;

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

int dumpGraphTree(tree_t* tree)
{
    CHECK(tree !=  NULL, ERR_TREE_NULL_PTR);

    FILE* file_dot = fopen("graphdump.dot", "wb");
    CHECK(file_dot !=  NULL, ERR_TREE_BAD_FILE);

    fprintf(file_dot, "digraph {\n");
    fprintf(file_dot, "\tnode[shape = \"cube\", color = \"#800000\", fontsize = 15, style = \"filled\", fillcolor = \"#88CEFA\"];\n"
                      "\tedge[color = \"#190970\", fontsize = 11];\n");

    dumpGraphNode(tree->root, file_dot);
    fprintf(file_dot, "}");

    fclose(file_dot);

    system("dot -Tjpeg -ographdump.jpeg graphdump.dot");
    system("convert graph_log.jpeg graphdump.jpeg -append graph_array.jpeg");
    system("gwenview graph_array.jpeg");

    return TREE_SUCCESS;
}

//=========================================================================

int dumpGraphNode(node_t* node, FILE* dot_out)
{
    CHECK(node    !=  NULL, ERR_TREE_NULL_PTR);
    CHECK(dot_out !=  NULL, ERR_TREE_BAD_FILE);

    switch(node->type)
    {
    case NUM:
        fprintf(dot_out, "\n\t\t\"%d_%p\"[shape = \"ellipse\", label = \"%d\", color=\"#900000\", style=\"filled\", \
                           fillcolor = \"#D0FDFF\"];\n", node->data.intValue, node, node->data.intValue);
        break;

    case VAR:
        fprintf(dot_out, "\n\t\t\"%c_%p\"[shape = \"ellipse\", label = \"%c\", color=\"#900000\", style=\"filled\", \
                           fillcolor = \"#D0FDFF\"];\n", *node->data.varValue, node, *node->data.varValue);
        break;

    case OP:
        switch(node->data.opValue)
        {
        case OP_ERROR:
            break;

        case OP_ADD:
            fprintf(dot_out, "\n\t\t\"+_%p\"[shape = \"ellipse\", label = \"+\", color=\"#900000\", \
                               style=\"filled\", fillcolor = \"#D0FDFF\"];\n", node);
            break;

        case OP_SUB:
            fprintf(dot_out, "\n\t\t\"-_%p\"[shape = \"ellipse\", label = \"-\", color=\"#900000\", \
                               style=\"filled\", fillcolor = \"#D0FDFF\"];\n", node);
            break;

        case OP_MUL:
            fprintf(dot_out, "\n\t\t\"*_%p\"[shape = \"ellipse\", label = \"*\", color=\"#900000\", \
                               style=\"filled\", fillcolor = \"#D0FDFF\"];\n", node);
            break;

        case OP_DIV:
            fprintf(dot_out, "\n\t\t\"/_%p\"[shape = \"ellipse\", label = \"/\", color=\"#900000\", \
                               style=\"filled\", fillcolor = \"#D0FDFF\"];\n", node);
            break;

        case OP_POW:
            fprintf(dot_out, "\n\t\t\"^_%p\"[shape = \"ellipse\", label = \"^\", color=\"#900000\", \
                               style=\"filled\", fillcolor = \"#D0FDFF\"];\n", node);
            break;

        case OP_SIN:
            fprintf(dot_out, "\n\t\t\"sin_%p\"[shape = \"ellipse\", label = \"sin\", color=\"#900000\", \
                               style=\"filled\", fillcolor = \"#D0FDFF\"];\n", node);
            break;

        case OP_COS:
            fprintf(dot_out, "\n\t\t\"cos_%p\"[shape = \"ellipse\", label = \"cos\", color=\"#900000\", \
                               style=\"filled\", fillcolor = \"#D0FDFF\"];\n", node);
            break;

        case OP_EXP:
            fprintf(dot_out, "\n\t\t\"exp_%p\"[shape = \"ellipse\", label = \"exp\", color=\"#900000\", \
                               style=\"filled\", fillcolor = \"#D0FDFF\"];\n", node);
            break;

        case OP_LN:
            fprintf(dot_out, "\n\t\t\"ln_%p\"[shape = \"ellipse\", label = \"ln\", color=\"#900000\", \
                               style=\"filled\", fillcolor = \"#D0FDFF\"];\n", node);
            break;            

        case OP_OPENBRT:
            fprintf(dot_out, "\n\t\t\"(_%p\"[shape = \"ellipse\", label = \"(\", color=\"#900000\", \
                               style=\"filled\", fillcolor = \"#D0FDFF\"];\n", node);
            break;  

        case OP_CLOSBRT:
            fprintf(dot_out, "\n\t\t\")_%p\"[shape = \"ellipse\", label = \")\", color=\"#900000\", \
                               style=\"filled\", fillcolor = \"#D0FDFF\"];\n", node);
            break; 

        case OP_CONNECT:
            fprintf(dot_out, "\n\t\t\"con_%p\"[shape = \"ellipse\", label = \"con\", color=\"#900000\", \
                               style=\"filled\", fillcolor = \"#D0FDFF\"];\n", node);
            break; 

        case OP_EQUAL:
            fprintf(dot_out, "\n\t\t\"=_%p\"[shape = \"ellipse\", label = \"=\", color=\"#900000\", \
                               style=\"filled\", fillcolor = \"#D0FDFF\"];\n", node);
            break;  

        case OP_MORE:
            fprintf(dot_out, "\n\t\t\">_%p\"[shape = \"ellipse\", label = \">\", color=\"#900000\", \
                               style=\"filled\", fillcolor = \"#D0FDFF\"];\n", node);
            break; 

        case OP_LESS:
            fprintf(dot_out, "\n\t\t\"<_%p\"[shape = \"ellipse\", label = \"<\", color=\"#900000\", \
                               style=\"filled\", fillcolor = \"#D0FDFF\"];\n", node);
            break;

        case OP_COMMA:
            fprintf(dot_out, "\n\t\t\",_%p\"[shape = \"ellipse\", label = \",\", color=\"#900000\", \
                               style=\"filled\", fillcolor = \"#D0FDFF\"];\n", node);
            break;

        case OP_SQRT:
            fprintf(dot_out, "\n\t\t\"sqrt_%p\"[shape = \"ellipse\", label = \"sqrt\", color=\"#900000\", \
                               style=\"filled\", fillcolor = \"#D0FDFF\"];\n", node);
            break;

        case OP_AND:
            fprintf(dot_out, "\n\t\t\"&_%p\"[shape = \"ellipse\", label = \"&\", color=\"#900000\", \
                               style=\"filled\", fillcolor = \"#D0FDFF\"];\n", node);
            break;

        case OP_OR:
            fprintf(dot_out, "\n\t\t\"|_%p\"[shape = \"ellipse\", label = \"|\", color=\"#900000\", \
                               style=\"filled\", fillcolor = \"#D0FDFF\"];\n", node);
            break;

        default:
            break;
        }

        if(node->left != NULL)
        {
            fprintfConnection(node, node->left, node->data.opValue, dot_out);
            dumpGraphNode(node->left, dot_out);
        }
        if(node->right != NULL)
        {
            fprintfConnection(node, node->right, node->data.opValue, dot_out);
            dumpGraphNode(node->right, dot_out);
        }
        break;

    case KEY:
        switch(node->data.keyValue)
        {
        case KEY_START:
            fprintf(dot_out, "\n\t\t\"start_%p\"[shape = \"ellipse\", label = \"start\", color=\"#800000\", \
                               style=\"filled\", fillcolor = \"#D0FDFF\"];\n", node);
            break;

        case KEY_IF:
            fprintf(dot_out, "\n\t\t\"if_%p\"[shape = \"ellipse\", label = \"if\", color=\"#800000\", \
                               style=\"filled\", fillcolor = \"#D0FDFF\"];\n", node);
            break;

        case KEY_WHILE:
            fprintf(dot_out, "\n\t\t\"while_%p\"[shape = \"ellipse\", label = \"while\", color=\"#800000\", \
                               style=\"filled\", fillcolor = \"#D0FDFF\"];\n", node);
            break;

        case KEY_ASSIGN:
            fprintf(dot_out, "\n\t\t\":=_%p\"[shape = \"ellipse\", label = \":=\", color=\"#800000\", \
                               style=\"filled\", fillcolor = \"#D0FDFF\"];\n", node);
            break;

        case KEY_FUNC:
            fprintf(dot_out, "\n\t\t\"%s_%p\"[shape = \"ellipse\", label = \"%s\", color=\"#800000\", \
                               style=\"filled\", fillcolor = \"#D0FDFF\"];\n", node->name, node, node->name);
            break;

        case KEY_RET:
            fprintf(dot_out, "\n\t\t\"ret_%p\"[shape = \"ellipse\", label = \"ret\", color=\"#800000\", \
                               style=\"filled\", fillcolor = \"#D0FDFF\"];\n", node);
            break;

        default:
            break;      
        }    

        if(node->left != NULL)
        {
            fprintfConnection(node, node->left, node->data.keyValue, dot_out);
            dumpGraphNode(node->left, dot_out);
        }
        if(node->right != NULL)
        {
            fprintfConnection(node, node->right, node->data.keyValue, dot_out);
            dumpGraphNode(node->right, dot_out);
        }
        break;

    default:
        break; 
    }

    return TREE_SUCCESS;
}

//=========================================================================

int fprintfConnection(node_t* node_prev, node_t* node, int operation, FILE* dot_out)
{
    CHECK(node    !=  NULL, ERR_TREE_NULL_PTR);
    CHECK(dot_out !=  NULL, ERR_TREE_BAD_FILE);

    switch(node->type)
    {
    case NUM:
        switch(operation)
        {
        case OP_ADD:
            fprintf(dot_out, "\t\t\"+_%p\"->\"%d_%p\";\n", node_prev, node->data.intValue, node);
            break;
        
        case OP_SUB:
            fprintf(dot_out, "\t\t\"-_%p\"->\"%d_%p\";\n", node_prev, node->data.intValue, node);
            break;

        case OP_MUL:
            fprintf(dot_out, "\t\t\"*_%p\"->\"%d_%p\";\n", node_prev, node->data.intValue, node);
            break;

        case OP_DIV:
            fprintf(dot_out, "\t\t\"/_%p\"->\"%d_%p\";\n", node_prev, node->data.intValue, node);
            break;

        case OP_POW:
            fprintf(dot_out, "\t\t\"^_%p\"->\"%d_%p\";\n", node_prev, node->data.intValue, node);
            break;

        case OP_SIN:
            fprintf(dot_out, "\t\t\"sin_%p\"->\"%d_%p\";\n", node_prev, node->data.intValue, node);
            break;

        case OP_COS:
            fprintf(dot_out, "\t\t\"cos_%p\"->\"%d_%p\";\n", node_prev, node->data.intValue, node);
            break;

        case OP_EXP:
            fprintf(dot_out, "\t\t\"exp_%p\"->\"%d_%p\";\n", node_prev, node->data.intValue, node);
            break;

        case OP_LN:
            fprintf(dot_out, "\t\t\"ln_%p\"->\"%d_%p\";\n", node_prev, node->data.intValue, node);
            break;

        case OP_OPENBRT:
            fprintf(dot_out, "\t\t\"(_%p\"->\"%d_%p\";\n", node_prev, node->data.intValue, node);
            break;  

        case OP_CLOSBRT:
            fprintf(dot_out, "\t\t\")_%p\"->\"%d_%p\";\n", node_prev, node->data.intValue, node);
            break;

        case KEY_ASSIGN:
            fprintf(dot_out, "\t\t\":=_%p\"->\"%d_%p\";\n", node_prev, node->data.intValue, node);
            break;

        case OP_EQUAL:
            fprintf(dot_out, "\t\t\"=_%p\"->\"%d_%p\";\n", node_prev, node->data.intValue, node);
            break;

        case OP_MORE:
            fprintf(dot_out, "\t\t\">_%p\"->\"%d_%p\";\n", node_prev, node->data.intValue, node);
            break;

        case OP_LESS:
            fprintf(dot_out, "\t\t\"<_%p\"->\"%d_%p\";\n", node_prev, node->data.intValue, node);
            break;

        case KEY_RET:
            fprintf(dot_out, "\t\t\"ret_%p\"->\"%d_%p\";\n", node_prev, node->data.intValue, node);
            break;

        case OP_SQRT:
            fprintf(dot_out, "\t\t\"sqrt_%p\"->\"%d_%p\";\n", node_prev, node->data.intValue, node);
            break;

        case KEY_FUNC:
            fprintf(dot_out, "\t\t\"%s_%p\"->\"%d_%p\";\n", node_prev->name, node_prev, node->data.intValue, node);
            break;

        default:
            break;
        }
        break;

    case VAR:
        switch(operation)
        {
        case OP_ADD:
            fprintf(dot_out, "\t\t\"+_%p\"->\"%c_%p\";\n", node_prev, *node->data.varValue, node);            
            break;
        
        case OP_SUB:
            fprintf(dot_out, "\t\t\"-_%p\"->\"%c_%p\";\n", node_prev, *node->data.varValue, node);
            break;

        case OP_MUL:
            fprintf(dot_out, "\t\t\"*_%p\"->\"%c_%p\";\n", node_prev, *node->data.varValue, node); 
            break;

        case OP_DIV:
            fprintf(dot_out, "\t\t\"/_%p\"->\"%c_%p\";\n", node_prev, *node->data.varValue, node); 
            break;

        case OP_POW:
            fprintf(dot_out, "\t\t\"^_%p\"->\"%c_%p\";\n", node_prev, *node->data.varValue, node); 
            break;

        case OP_SIN:
            fprintf(dot_out, "\t\t\"sin_%p\"->\"%c_%p\";\n", node_prev, *node->data.varValue, node);
            break;

        case OP_COS:
            fprintf(dot_out, "\t\t\"cos_%p\"->\"%c_%p\";\n", node_prev, *node->data.varValue, node);
            break;

        case OP_EXP:
            fprintf(dot_out, "\t\t\"exp_%p\"->\"%c_%p\";\n", node_prev, *node->data.varValue, node);
            break;

        case OP_LN:
            fprintf(dot_out, "\t\t\"ln_%p\"->\"%c_%p\";\n", node_prev, *node->data.varValue, node);
            break;

        case OP_OPENBRT:
            fprintf(dot_out, "\t\t\"(_%p\"->\"%c_%p\";\n", node_prev, *node->data.varValue, node);
            break;  

        case OP_CLOSBRT:
            fprintf(dot_out, "\t\t\")_%p\"->\"%c_%p\";\n", node_prev, *node->data.varValue, node);
            break;

        case KEY_ASSIGN:
            fprintf(dot_out, "\t\t\":=_%p\"->\"%c_%p\";\n", node_prev, *node->data.varValue, node);
            break;

        case OP_EQUAL:
            fprintf(dot_out, "\t\t\"=_%p\"->\"%c_%p\";\n", node_prev, *node->data.varValue, node);
            break;

        case OP_MORE:
            fprintf(dot_out, "\t\t\">_%p\"->\"%c_%p\";\n", node_prev, *node->data.varValue, node);
            break;

        case OP_LESS:
            fprintf(dot_out, "\t\t\"<_%p\"->\"%c_%p\";\n", node_prev, *node->data.varValue, node);
            break;

        case OP_COMMA:
            fprintf(dot_out, "\t\t\",_%p\"->\"%c_%p\";\n", node_prev, *node->data.varValue, node);
            break;

        case KEY_RET:
            fprintf(dot_out, "\t\t\"ret_%p\"->\"%c_%p\";\n", node_prev, *node->data.varValue, node);
            break;

        case KEY_FUNC:
            fprintf(dot_out, "\t\t\"%s_%p\"->\"%c_%p\";\n", node_prev->name, node_prev, *node->data.varValue, node);
            break;

        case OP_SQRT:
            fprintf(dot_out, "\t\t\"sqrt_%p\"->\"%c_%p\";\n", node_prev, *node->data.varValue, node);
            break;

        default:
            break;
        }
        break;

    case OP:
        switch(operation)
        {
        case OP_ADD:
            fprintf(dot_out, "\t\t\"+_%p\"->", node_prev);
            break;
        
        case OP_SUB:
            fprintf(dot_out, "\t\t\"-_%p\"->", node_prev);
            break;

        case OP_MUL:
            fprintf(dot_out, "\t\t\"*_%p\"->", node_prev);
            break;

        case OP_DIV:
            fprintf(dot_out, "\t\t\"/_%p\"->", node_prev);
            break;

        case OP_POW:
            fprintf(dot_out, "\t\t\"^_%p\"->", node_prev);
            break;

        case OP_SIN:
            fprintf(dot_out, "\t\t\"sin_%p\"->", node_prev);
            break;

        case OP_COS:
            fprintf(dot_out, "\t\t\"cos_%p\"->", node_prev);
            break;

        case OP_EXP:
            fprintf(dot_out, "\t\t\"exp_%p\"->", node_prev);
            break;

        case OP_LN:
            fprintf(dot_out, "\t\t\"ln_%p\"->", node_prev);
            break;

        case OP_OPENBRT:
            fprintf(dot_out, "\t\t\"(_%p\"->", node_prev);
            break;  

        case OP_CLOSBRT:
            fprintf(dot_out, "\t\t\")_%p\"->", node_prev);
            break;

        case OP_CONNECT:
            fprintf(dot_out, "\t\t\"con_%p\"->", node_prev);
            break;

        case OP_EQUAL:
            fprintf(dot_out, "\t\t\"=_%p\"->", node_prev);
            break;

        case OP_MORE:
            fprintf(dot_out, "\t\t\">_%p\"->", node_prev);
            break;

        case OP_LESS:
            fprintf(dot_out, "\t\t\"<_%p\"->", node_prev);
            break;

        case KEY_START:
            fprintf(dot_out, "\t\t\"start_%p\"->", node_prev);
            break;

        case KEY_ASSIGN:
            fprintf(dot_out, "\t\t\":=_%p\"->", node_prev);
            break;

        case KEY_IF:
            fprintf(dot_out, "\t\t\"if_%p\"->", node_prev);
            break;

        case KEY_WHILE:
            fprintf(dot_out, "\t\t\"while_%p\"->", node_prev);
            break;

        case KEY_FUNC:
            fprintf(dot_out, "\t\t\"%s_%p\"->", node_prev->name, node_prev);
            break;

        case OP_COMMA:
            fprintf(dot_out, "\t\t\",_%p\"->", node_prev);
            break;

        case KEY_RET:
            fprintf(dot_out, "\t\t\"ret_%p\"->", node_prev);
            break;

        case OP_SQRT:
            fprintf(dot_out, "\t\t\"sqrt_%p\"->", node_prev);
            break;

        case OP_AND:
            fprintf(dot_out, "\t\t\"&_%p\"->", node_prev);
            break;

        case OP_OR:
            fprintf(dot_out, "\t\t\"|_%p\"->", node_prev);
            break;

        default:
            break;
        }

        switch(node->data.opValue)
        {
        case OP_ADD:
            fprintf(dot_out, "\"+_%p\";\n", node);
            break;
        
        case OP_SUB:
            fprintf(dot_out, "\"-_%p\";\n", node);
            break;

        case OP_MUL:
            fprintf(dot_out, "\"*_%p\";\n", node);
            break;

        case OP_DIV:
            fprintf(dot_out, "\"/_%p\";\n", node);
            break;

        case OP_POW:
            fprintf(dot_out, "\"^_%p\";\n", node);
            break;

        case OP_SIN:
            fprintf(dot_out, "\"sin_%p\";\n", node);
            break;

        case OP_COS:
            fprintf(dot_out, "\"cos_%p\";\n", node);
            break;

        case OP_EXP:
            fprintf(dot_out, "\"exp_%p\";\n", node);
            break;

        case OP_LN:
            fprintf(dot_out, "\"ln_%p\";\n", node);
            break;

        case OP_OPENBRT:
            fprintf(dot_out, "\"(_%p\";\n", node);
            break;  

        case OP_CLOSBRT:
            fprintf(dot_out, "\")_%p\";\n", node);
            break;

        case OP_EQUAL:
            fprintf(dot_out, "\"=_%p\";\n", node);
            break;

        case OP_MORE:
            fprintf(dot_out, "\">_%p\";\n", node);
            break;

        case OP_LESS:
            fprintf(dot_out, "\"<_%p\";\n", node);
            break;

        case OP_CONNECT:
            fprintf(dot_out, "\"con_%p\";\n", node);
            break;

        case OP_COMMA:
            fprintf(dot_out, "\",_%p\";\n", node);
            break;

        case OP_SQRT:
            fprintf(dot_out, "\"sqrt_%p\";\n", node);
            break;

        case OP_AND:
            fprintf(dot_out, "\"&_%p\";\n", node);
            break;

        case OP_OR:
            fprintf(dot_out, "\"|_%p\";\n", node);
            break;

        default:
            break;
        }
        break;

    case KEY:
        switch(operation)
        {
        case KEY_START:
            fprintf(dot_out, "\t\t\"start_%p\"->", node_prev);
            break;

        case KEY_WHILE:
            fprintf(dot_out, "\t\t\"while_%p\"->", node_prev);
            break;

        case KEY_IF:
            fprintf(dot_out, "\t\t\"if_%p\"->", node_prev);
            break;

        case OP_CONNECT:
            fprintf(dot_out, "\t\t\"con_%p\"->", node_prev);
            break;

        case KEY_FUNC:
            fprintf(dot_out, "\t\t\"%s_%p\"->", node_prev->name, node_prev);
            break;

        case KEY_ASSIGN:
            fprintf(dot_out, "\t\t\":=_%p\"->", node_prev);
            break;

        case OP_AND:
            fprintf(dot_out, "\t\t\"&_%p\"->", node_prev);
            break;

        case OP_OR:
            fprintf(dot_out, "\t\t\"|_%p\"->", node_prev);
            break;
        default:
            break;
        }

        switch(node->data.opValue)
        {
        case OP_CONNECT:
            fprintf(dot_out, "\"con_%p\";\n", node);
            break;

        default:
            break;        
        }  
        switch(node->data.keyValue)
        {
        case KEY_WHILE:
            fprintf(dot_out, "\"while_%p\";\n", node);
            break;

        case KEY_IF:
            fprintf(dot_out, "\"if_%p\";\n", node);
            break;

        case KEY_ASSIGN:
            fprintf(dot_out, "\":=_%p\";\n", node);
            break;

        case KEY_FUNC:
            fprintf(dot_out, "\"%s_%p\";\n", node->name, node);
            break;

        case KEY_RET:
            fprintf(dot_out, "\t\t\"ret_%p\";\n", node);
            break;

        default:
            break;       
        }   

    default:
        break;
    }

    return TREE_SUCCESS;
}
