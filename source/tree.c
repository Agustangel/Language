#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lang.h"
#include "debug.h"
#include "tree.h"


//=========================================================================

int treeCtor(tree_t* tree, name_t name_program)
{
    CHECK(tree !=  NULL, ERR_TREE_NULL_PTR);

    tree->root = NULL;
    tree->status = TREE_SUCCESS;
    tree->name_equation = getNameFile(name_program);

    return TREE_SUCCESS;
}

//=========================================================================

name_t getNameFile(name_t file)
{
    CHECK(file !=  NULL, NULL);

    char* begin = file;
    while ((strchr(begin, '/')) != NULL) 
    {
        begin = strchr(begin, '/') + 1;
    }
    name_t name_file = strdup(begin);

    char* pointer_format  = strchr(name_file, '.');
    if(pointer_format != NULL)
    {
        *pointer_format = '\0';
    }

    return name_file;
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
    node->left  = (prev_node->left  != NULL) ? copyNode(prev_node->left)  : NULL;
    node->right = (prev_node->right != NULL) ? copyNode(prev_node->right) : NULL;

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
    tree->name_equation = NAME_POISON;

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
            MAKE_FRMT_MSG(+);
            break;

        case OP_SUB:
            MAKE_FRMT_MSG(-);
            break;

        case OP_MUL:
            MAKE_FRMT_MSG(*);
            break;

        case OP_DIV:
            MAKE_FRMT_MSG(/);
            break;

        case OP_POW:
            MAKE_FRMT_MSG(^);
            break;

        case OP_SIN:
            MAKE_FRMT_MSG(sin);
            break;

        case OP_COS:
            MAKE_FRMT_MSG(cos);
            break;

        case OP_EXP:
            MAKE_FRMT_MSG(exp);
            break;

        case OP_LN:
            MAKE_FRMT_MSG(ln);
            break;
                               
        case OP_SQRT:
            MAKE_FRMT_MSG(sqrt);
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
            MAKE_FRMT_MSG(con);
            break; 

        case OP_EQUAL:
            MAKE_FRMT_MSG(=);
            break;  

        case OP_MORE:
            MAKE_FRMT_MSG(>);
            break; 

        case OP_LESS:
            MAKE_FRMT_MSG(<);
            break;

        case OP_COMMA:
            fprintf(dot_out, "\n\t\t\",_%p\"[shape = \"ellipse\", label = \",\", color=\"#900000\", \
                               style=\"filled\", fillcolor = \"#D0FDFF\"];\n", node);
            break;

        case OP_AND:
            MAKE_FRMT_MSG(&);
            break;

        case OP_OR:
            MAKE_FRMT_MSG(|);
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
        case KEY_DEC:
            MAKE_FRMT_MSG(dec);
            break;

        case KEY_IF:
            MAKE_FRMT_MSG(if);
            break;

        case KEY_WHILE:
            MAKE_FRMT_MSG(while);
            break;

        case KEY_ASSIGN:
            MAKE_FRMT_MSG(:=);
            break;

        case KEY_FUNC:
            fprintf(dot_out, "\n\t\t\"%s_%p\"[shape = \"ellipse\", label = \"%s\", color=\"#800000\", \
                               style=\"filled\", fillcolor = \"#D0FDFF\"];\n", node->name, node, node->name);
            break;

        case KEY_RET:
            MAKE_FRMT_MSG(ret);;
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
            MAKE_CON_NUM(+);
            break;
        
        case OP_SUB:
            MAKE_CON_NUM(-);
            break;

        case OP_MUL:
            MAKE_CON_NUM(*);
            break;

        case OP_DIV:
            MAKE_CON_NUM(/);
            break;

        case OP_POW:
            MAKE_CON_NUM(^);
            break;

        case OP_SIN:
            MAKE_CON_NUM(sin);
            break;

        case OP_COS:
            MAKE_CON_NUM(cos);
            break;

        case OP_EXP:
            MAKE_CON_NUM(exp);
            break;

        case OP_LN:
            MAKE_CON_NUM(ln);
            break;

        case OP_SQRT:
            MAKE_CON_NUM(sqrt);
            break;

        case OP_OPENBRT:
            fprintf(dot_out, "\t\t\"(_%p\"->\"%d_%p\";\n", node_prev, node->data.intValue, node);
            break;  

        case OP_CLOSBRT:
            fprintf(dot_out, "\t\t\")_%p\"->\"%d_%p\";\n", node_prev, node->data.intValue, node);
            break;

        case OP_COMMA:
            fprintf(dot_out, "\t\t\",_%p\"->\"%d_%p\";\n", node_prev, node->data.intValue, node);
            break;

        case KEY_ASSIGN:
            MAKE_CON_NUM(:=);
            break;

        case OP_EQUAL:
            MAKE_CON_NUM(=);
            break;

        case OP_MORE:
            MAKE_CON_NUM(>);
            break;

        case OP_LESS:
            MAKE_CON_NUM(<);
            break;

        case KEY_RET:
            MAKE_CON_NUM(ret);
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
            MAKE_CON_VAR(+);            
            break;
        
        case OP_SUB:
            MAKE_CON_VAR(-);
            break;

        case OP_MUL:
            MAKE_CON_VAR(*); 
            break;

        case OP_DIV:
            MAKE_CON_VAR(/);
            break;

        case OP_POW:
            MAKE_CON_VAR(^);
            break;

        case OP_SIN:
            MAKE_CON_VAR(sin);
            break;

        case OP_COS:
            MAKE_CON_VAR(cos);
            break;

        case OP_EXP:
            MAKE_CON_VAR(exp);
            break;

        case OP_LN:
            MAKE_CON_VAR(ln);
            break;

        case OP_SQRT:
            MAKE_CON_VAR(sqrt);
            break;

        case OP_OPENBRT:
            fprintf(dot_out, "\t\t\"(_%p\"->\"%c_%p\";\n", node_prev, *node->data.varValue, node);
            break;  

        case OP_CLOSBRT:
            fprintf(dot_out, "\t\t\")_%p\"->\"%c_%p\";\n", node_prev, *node->data.varValue, node);
            break;

        case KEY_ASSIGN:
            MAKE_CON_VAR(:=);
            break;

        case OP_EQUAL:
            MAKE_CON_VAR(=);
            break;

        case OP_MORE:
            MAKE_CON_VAR(>);
            break;

        case OP_LESS:
            MAKE_CON_VAR(<);
            break;

        case OP_COMMA:
            fprintf(dot_out, "\t\t\",_%p\"->\"%c_%p\";\n", node_prev, *node->data.varValue, node);
            break;

        case KEY_RET:
            MAKE_CON_VAR(ret);
            break;

        case KEY_FUNC:
            fprintf(dot_out, "\t\t\"%s_%p\"->\"%c_%p\";\n", node_prev->name, node_prev, *node->data.varValue, node);
            break;

        default:
            break;
        }
        break;

    case OP:
        switch(operation)
        {
        case OP_ADD:
            MAKE_CON_OPB(+);
            break;
        
        case OP_SUB:
            MAKE_CON_OPB(-);
            break;

        case OP_MUL:
            MAKE_CON_OPB(*);
            break;

        case OP_DIV:
            MAKE_CON_OPB(/);
            break;

        case OP_POW:
            MAKE_CON_OPB(^);
            break;

        case OP_SIN:
            MAKE_CON_OPB(sin);
            break;

        case OP_COS:
            MAKE_CON_OPB(cos);
            break;

        case OP_EXP:
            MAKE_CON_OPB(exp);
            break;

        case OP_LN:
            MAKE_CON_OPB(ln);
            break;

        case OP_SQRT:
            MAKE_CON_OPB(sqrt);
            break;

        case OP_OPENBRT:
            fprintf(dot_out, "\t\t\"(_%p\"->", node_prev);
            break;  

        case OP_CLOSBRT:
            fprintf(dot_out, "\t\t\")_%p\"->", node_prev);
            break;

        case OP_CONNECT:
            MAKE_CON_OPB(con);
            break;

        case OP_EQUAL:
            MAKE_CON_OPB(=);
            break;

        case OP_MORE:
            MAKE_CON_OPB(>);
            break;

        case OP_LESS:
            MAKE_CON_OPB(<);
            break;

        case KEY_ASSIGN:
            MAKE_CON_KEYB(:=);
            break;

        case KEY_IF:
            MAKE_CON_KEYB(if);
            break;

        case KEY_WHILE:
            MAKE_CON_KEYB(while);
            break;

        case KEY_FUNC:
            fprintf(dot_out, "\t\t\"%s_%p\"->", node_prev->name, node_prev);
            break;

        case OP_COMMA:
            fprintf(dot_out, "\t\t\",_%p\"->", node_prev);
            break;

        case KEY_RET:
            MAKE_CON_KEYB(ret);
            break;

        case OP_AND:
            MAKE_CON_OPB(&);
            break;

        case OP_OR:
            MAKE_CON_OPB(|);
            break;

        case KEY_DEC:
            MAKE_CON_OPB(dec);
            break;

        default:
            break;
        }

        switch(node->data.opValue)
        {
        case OP_ADD:
            MAKE_CON_OPE(+);
            break;
        
        case OP_SUB:
            MAKE_CON_OPE(-);
            break;

        case OP_MUL:
            MAKE_CON_OPE(*);
            break;

        case OP_DIV:
            MAKE_CON_OPE(/);
            break;

        case OP_POW:
            MAKE_CON_OPE(^);
            break;

        case OP_SIN:
            MAKE_CON_OPE(sin);
            break;

        case OP_COS:
            MAKE_CON_OPE(cos);
            break;

        case OP_EXP:
            MAKE_CON_OPE(exp);
            break;

        case OP_LN:
            MAKE_CON_OPE(ln);
            break;

        case OP_SQRT:
            MAKE_CON_OPE(sqrt);
            break;

        case OP_OPENBRT:
            fprintf(dot_out, "\"(_%p\";\n", node);
            break;  

        case OP_CLOSBRT:
            fprintf(dot_out, "\")_%p\";\n", node);
            break;

        case OP_EQUAL:
            MAKE_CON_OPE(=);
            break;

        case OP_MORE:
            MAKE_CON_OPE(>);
            break;

        case OP_LESS:
            MAKE_CON_OPE(<);
            break;

        case OP_CONNECT:
            MAKE_CON_OPE(con);
            break;

        case OP_COMMA:
            fprintf(dot_out, "\",_%p\";\n", node);
            break;

        case OP_AND:
            MAKE_CON_OPE(&);
            break;

        case OP_OR:
            MAKE_CON_OPE(|);
            break;

        default:
            break;
        }
        break;

    case KEY:
        switch(operation)
        {
        case KEY_DEC:
            MAKE_CON_KEYB(dec);
            break;

        case KEY_ASSIGN:
            MAKE_CON_KEYB(:=);
            break;

        case KEY_IF:
            MAKE_CON_KEYB(if);
            break;

        case KEY_WHILE:
            MAKE_CON_KEYB(while);
            break;

        case OP_CONNECT:
            fprintf(dot_out, "\t\t\"con_%p\"->", node_prev);
            break;

        case KEY_FUNC:
            fprintf(dot_out, "\t\t\"%s_%p\"->", node_prev->name, node_prev);
            break;

        case OP_AND:
            MAKE_CON_OPB(&);
            break;

        case OP_OR:
            MAKE_CON_OPB(|);
            break;

        default:
            break;
        }

        switch(node->data.opValue)
        {
        case OP_CONNECT:
            MAKE_CON_OPE(con);
            break;

        default:
            break;        
        }  
        switch(node->data.keyValue)
        {
        case KEY_DEC:
            MAKE_CON_KEYE(dec);
            break;

        case KEY_WHILE:
            MAKE_CON_KEYE(while);
            break;

        case KEY_IF:
            MAKE_CON_KEYE(if);
            break;

        case KEY_ASSIGN:
            MAKE_CON_KEYE(:=);
            break;

        case KEY_FUNC:
            fprintf(dot_out, "\"%s_%p\";\n", node->name, node);
            break;

        case KEY_RET:
            MAKE_CON_KEYE(ret);
            break;

        default:
            break;       
        }   

    default:
        break;
    }

    return TREE_SUCCESS;
}
