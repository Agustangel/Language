#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "lang.h"
#include "debug.h"
#include "tree.h"



//=========================================================================

int parseArgs(int argc, char* argv[], name_t* name_program)
{
    CHECK(argv  !=  NULL, ERR_LANG_NULL_PTR);
    CHECK(argc  > 0, ERR_LANG_NEGATIVE_COUNT);

    if(argc < 2)
    {
        printf("Input name of the program\n");
        return ERR_LANG_NO_INPUT;
    }
    if (argc == 2)
    {
        *name_program = argv[1];
    }

    return LANG_SUCCESS;
}

//=========================================================================

int programCtor(FILE* file, program_t* program)
{
    CHECK(file     !=  NULL, ERR_LANG_NULL_PTR);
    CHECK(program  !=  NULL, ERR_LANG_NULL_PTR);

    size_t size = count_symbols(file);;

    program->buffer = (char*) calloc(size, sizeof(char));
    CHECK(program->buffer !=  NULL, ERR_LANG_NULL_PTR);
    program->current_symbol = program->buffer;

    fseek(file, 0, SEEK_SET);
    fread(program->buffer, sizeof(char), size, file);

    return LANG_SUCCESS;
}

//=========================================================================

int programDtor(program_t* program)
{
    CHECK(program  !=  NULL, ERR_LANG_NULL_PTR);

    free(program->buffer);
    program->buffer         = NULL;
    program->current_symbol = NULL;

    return LANG_SUCCESS;
}

//=========================================================================

size_t skipSeparator(char** string)
{   
    CHECK(string  !=  NULL, ERR_LANG_NULL_PTR);

    size_t count = 0;
    while(isspace((int)(unsigned char)**string))
    {
        ++(*string);
        ++count;
    }

    return count; 
}

//=========================================================================

node_t* makeAST(program_t* program)
{
    CHECK(program != NULL, NULL);

    bool start_main = false;

    node_t* root  = getGlobal(program);
    node_t* main  = getMain(program);
    node_t* start = NULL;
    if(root == NULL)
    {
        start_main = true;
        main = createNodeKey(KEY_DEC, main, createKey(KEY_DEC));
        treeNodeDtor(main->right);
        main->right = NULL;
        start = main;
    }
    else
    {
        start = findStart(root);
        start->left = main;
    }

    skipSeparator(&program->current_symbol);
    if(*program->current_symbol != '\0')
    {
        start->right = createKey(KEY_DEC);
        getNodeDec(program, &start->right, '\0');
    }
    CHECK(*program->current_symbol == '\0', NULL);
    
    if(start_main == true)
    {
        root = start;
    }

    return root;
}

//=========================================================================

void getNodeDec(program_t* program, node_t** node, char stop_symbol)
{
    CHECK(program != NULL, ;);
    CHECK(node    != NULL, ;);

    treeNodeDtor(*node);
    *node = NULL;

    skipSeparator(&program->current_symbol);
    if(*program->current_symbol == stop_symbol)
    {
        return;
    }
    if(strncmp(program->current_symbol, "func", 4) != 0)
    {
        return;
    }
    program->current_symbol += 4;

    node_t* val = getF(program);
    CHECK(val != NULL, ;);
    *node = val;

    skipSeparator(&program->current_symbol);
    if(*program->current_symbol != stop_symbol)
    {
        *node = createNodeKey(KEY_DEC, val, createKey(KEY_DEC));
        getNodeDec(program, &(*node)->right, stop_symbol);
    }
}

//=========================================================================

void getNodeS(program_t* program, node_t** node, char stop_symbol)
{
    CHECK(program != NULL, ;);
    CHECK(node    != NULL, ;);

    treeNodeDtor(*node);
    *node = NULL;

    skipSeparator(&program->current_symbol);
    if(*program->current_symbol == stop_symbol)
    {
        return;
    }

    node_t* val = getS(program);
    CHECK(val != NULL, ;);
    *node = val;

    skipSeparator(&program->current_symbol);
    if(*program->current_symbol != stop_symbol)
    {
        *node = createNodeOp(OP_CONNECT, val, createOp(OP_CONNECT));
        getNodeS(program, &(*node)->right, stop_symbol);
    }
}

//=========================================================================

node_t* getS(program_t* program)
{
    CHECK(program != NULL, NULL);

    skipSeparator(&program->current_symbol);
    node_t* val = NULL;

    if(strncmp(program->current_symbol, "if", 2) == 0)
    {
        program->current_symbol += 2;
        val = getIf(program);

        return val;
    }
    if(strncmp(program->current_symbol, "while", 5) == 0)
    {
        program->current_symbol += 5;
        val = getWhile(program);

        return val;        
    }
    if(strncmp(program->current_symbol, "ret", 3) == 0)
    {
        program->current_symbol += 3;
        val = getRet(program);

        return val;
    }
    if(strncmp(program->current_symbol, "func", 4) == 0)
    {
        program->current_symbol += 4;
        val = getF(program);

        return val;
    }

    val = callFunc(program);
    if(val == NULL)
    {
        --program->current_symbol;
        val = getN(program);
    }
    skipSeparator(&program->current_symbol);
    if(strncmp(program->current_symbol, ":=", 2) == 0)
    {
        val = getAs(program, val->data.varValue);

        return val;
    }
    
    return val; 
}

//=========================================================================

node_t* getN(program_t* program)
{
    CHECK(program != NULL, NULL);

    skipSeparator(&program->current_symbol);
    int tmp = 1;

    if(*program->current_symbol == '-')
    {
        ++program->current_symbol;
        tmp = -1;
    }
    if(isdigit(*program->current_symbol))
    {
        int val = 0;
        char* sOld = program->current_symbol;

        while(isdigit(*program->current_symbol))
        {
            val = (val * 10) + (*program->current_symbol - '0');
            ++program->current_symbol;
        }
        val *= tmp;
        CHECK(program->current_symbol > sOld, NULL);

        return createNum(val);
    }
    else
    {
        char* elem = program->current_symbol;
        ++program->current_symbol;

        if(tmp == -1)
        {
            return createNodeOp(OP_MUL, createVar(elem), createNum(-1));
        }
        return createVar(elem);
    }
}

//=========================================================================

node_t* getE(program_t* program)
{
    CHECK(program != NULL, NULL);

    skipSeparator(&program->current_symbol);
    node_t* val = getT(program);

    skipSeparator(&program->current_symbol);
    while((*program->current_symbol == '+') || (*program->current_symbol == '-'))
    {
        char op = *program->current_symbol;
        ++program->current_symbol;

        node_t* val_2 = getT(program);
        if(op == '+')
        {
            val = createNodeOp(OP_ADD, val, val_2);
        }
        else
        {
            val = createNodeOp(OP_SUB, val, val_2);
        }
    }

    return val;
}

//=========================================================================

node_t* getT(program_t* program)
{
    CHECK(program != NULL, NULL);

    skipSeparator(&program->current_symbol);
    node_t* val = getL(program);

    skipSeparator(&program->current_symbol);
    while((*program->current_symbol == '*') || (*program->current_symbol == '/'))
    {
        char op = *program->current_symbol;
        ++program->current_symbol;
        skipSeparator(&program->current_symbol);

        node_t* val_2 = getL(program);
        if(op == '*')
        {
            val = createNodeOp(OP_MUL, val, val_2);
        }
        else
        {
            val = createNodeOp(OP_DIV, val, val_2);
        }
    }

    return val;    
}

//=========================================================================

node_t* getP(program_t* program)
{
    CHECK(program != NULL, NULL);

    node_t* val = NULL;

    skipSeparator(&program->current_symbol);
    if(*program->current_symbol == '(')
    {
        ++program->current_symbol;
        val = getE(program);
        skipSeparator(&program->current_symbol);

        CHECK(*program->current_symbol == ')', NULL);
        ++program->current_symbol;

        return createNodeOp(OP_OPENBRT, val, createOp(OP_CLOSBRT));
    }
    val = getN(program);
    
    return val;
}

//=========================================================================

node_t* getL(program_t* program)
{
    CHECK(program != NULL, NULL);
    
    skipSeparator(&program->current_symbol);
    if(strncmp(program->current_symbol, "cos", 3) == 0)
    {
        program->current_symbol += 3;
        return createNodeOp(OP_COS, NULL, getP(program));
    }
    if(strncmp(program->current_symbol, "sin", 3) == 0)
    {
        program->current_symbol += 3;
        return createNodeOp(OP_SIN, NULL, getP(program));
    }
    if(strncmp(program->current_symbol, "exp", 3) == 0)
    {
        program->current_symbol += 3;
        return createNodeOp(OP_EXP, NULL, getP(program));        
    }
    if(strncmp(program->current_symbol, "ln", 2) == 0)
    {
        program->current_symbol += 2;
        return createNodeOp(OP_LN, NULL, getP(program));        
    }
    if(strncmp(program->current_symbol, "sqrt", 4) == 0)
    {
        program->current_symbol += 4;
        return createNodeOp(OP_SQRT, NULL, getP(program));        
    }

    node_t* val = getP(program);
    skipSeparator(&program->current_symbol);
    if(*program->current_symbol == '^')
    {
        ++program->current_symbol;
        node_t* power = getP(program);
        skipSeparator(&program->current_symbol);
        val = createNodeOp(OP_POW, val, power);
    }
    CHECK(*program->current_symbol != '^', NULL);
    
    return val;
}

//=========================================================================

node_t* getIf(program_t* program)
{
    CHECK(program != NULL, NULL);

    node_t* val = NULL;

    skipSeparator(&program->current_symbol);
    CHECK(*program->current_symbol == '(', NULL);
    ++program->current_symbol;

    node_t* val_1 = callFunc(program);
    if(val_1 == NULL)
    {
        --program->current_symbol;
        val_1 = getP(program);
    }
    skipSeparator(&program->current_symbol);
    if((*program->current_symbol == '>') || (*program->current_symbol == '<') || (*program->current_symbol == '=') || 
       (*program->current_symbol == '&') || (*program->current_symbol == '|'))
    {
        char op = *program->current_symbol;
        ++program->current_symbol;

        node_t* val_2 = getP(program);
        if(op == '>')
        {
            val_1 = createNodeOp(OP_MORE, val_1, val_2);
        }
        else if(op == '<')
        {
            val_1 = createNodeOp(OP_LESS, val_1, val_2);
        }
        else if(op == '=')
        {
            val_1 = createNodeOp(OP_EQUAL, val_1, val_2);
        }
        else if(op == '&')
        {
            val_1 = createNodeOp(OP_AND, val_1, val_2);
        }
        else if(op == '|')
        {
            val_1 = createNodeOp(OP_OR, val_1, val_2);
        }
    }
    CHECK(*program->current_symbol == ')', NULL);
    ++program->current_symbol;
    skipSeparator(&program->current_symbol);

    CHECK(*program->current_symbol == '[', NULL);
    ++program->current_symbol;
    
    val = getS(program);
    CHECK(val != NULL, NULL);

    skipSeparator(&program->current_symbol);
    if(*program->current_symbol != ']')
    {
        val = createNodeOp(OP_CONNECT, val, createOp(OP_CONNECT));
        getNodeS(program, &val->right, ']');
    }
    CHECK(*program->current_symbol == ']', NULL);
    ++program->current_symbol;

    val_1 = createNodeKey(KEY_IF, val_1, val);

    return val_1;
}

//=========================================================================

node_t* getWhile(program_t* program)
{
    CHECK(program != NULL, NULL);

    node_t* val = NULL;

    skipSeparator(&program->current_symbol);
    CHECK(*program->current_symbol == '(', NULL);
    ++program->current_symbol;

    node_t* val_1 = callFunc(program);
    if(val_1 == NULL)
    {
        --program->current_symbol;
        val_1 = getP(program);
    }
    skipSeparator(&program->current_symbol);
    if((*program->current_symbol == '>') || (*program->current_symbol == '<') || (*program->current_symbol == '='))
    {
        char op = *program->current_symbol;
        ++program->current_symbol;

        node_t* val_2 = getP(program);
        if(op == '>')
        {
            val_1 = createNodeOp(OP_MORE, val_1, val_2);
        }
        else if(op == '<')
        {
            val_1 = createNodeOp(OP_LESS, val_1, val_2);
        }
        else
        {
            val_1 = createNodeOp(OP_EQUAL, val_1, val_2);
        }
    }
    CHECK(*program->current_symbol == ')', NULL);
    ++program->current_symbol;
    skipSeparator(&program->current_symbol);

    CHECK(*program->current_symbol == '[', NULL);
    ++program->current_symbol;

    val = getS(program);
    CHECK(val != NULL, NULL);

    skipSeparator(&program->current_symbol);   
    if(*program->current_symbol != ']')
    {
        val = createNodeOp(OP_CONNECT, val, createOp(OP_CONNECT));
        getNodeS(program, &val->right, ']');
    }
    CHECK(*program->current_symbol == ']', NULL);
    ++program->current_symbol;

    val_1 = createNodeKey(KEY_WHILE, val_1, val);

    return val_1;
}

//=========================================================================

node_t* getAs(program_t* program, name_t name)
{
    CHECK(program != NULL, NULL);

    node_t* val = NULL;
    name_t var_name = name;

    program->current_symbol += 2;
    node_t* var_value = callFunc(program);
    if(var_value == NULL)
    {
        --program->current_symbol;
        var_value = getE(program);
    }

    val = createNodeKey(KEY_ASSIGN, createVar(var_name), var_value);

    return val;
}

//=========================================================================

node_t* getF(program_t* program)
{
    CHECK(program != NULL, NULL);

    node_t* func  = NULL;
    node_t* body  = NULL;

    char* name = getName(program);
    CHECK(name != NULL, NULL);

    node_t* param = getParam(program);
    CHECK(param  != NULL, NULL);
    skipSeparator(&program->current_symbol);
    
    CHECK(*program->current_symbol == '[', NULL);
    ++program->current_symbol;

    body = getS(program);
    CHECK(body != NULL, NULL);

    skipSeparator(&program->current_symbol); 
    if(*program->current_symbol != ']')
    {
        body = createNodeOp(OP_CONNECT, body, createOp(OP_CONNECT));
        getNodeS(program, &body->right, ']');
    }
    CHECK(*program->current_symbol == ']', NULL);
    ++program->current_symbol;

    if(param->data.opValue == OP_NOPARAM)
    {
        func = createNodeKey(KEY_FUNC, createOp(OP_COMMA), body);
        func->name = name;
        treeNodeDtor(func->left);
        func->left = NULL;

        return func;
    }
    func = createNodeKey(KEY_FUNC, param, body);
    func->name = name;

    return func;
}

//=========================================================================

char* getName(program_t* program)
{
    CHECK(program != NULL, NULL);

    int a = 0;

    int len = getLenName(program);
    CHECK(len > 0, NULL);

    char* name = (char*) calloc(len + 1, sizeof(char));
    memcpy(name, program->current_symbol, len * sizeof(char)); 
    name[len] = '\0';
    program->current_symbol += len;

    return name;
}

//=========================================================================

int getLenName(program_t* program)
{
    CHECK(program != NULL, ERR_LANG_NULL_PTR);

    skipSeparator(&program->current_symbol);

    int len = 0;
    char* tmp_ptr = program->current_symbol;
    while(isalpha(*tmp_ptr))
    {
        ++len;
        ++tmp_ptr;
    }

    return len;
}

//=========================================================================

node_t* getParam(program_t* program)
{
    CHECK(program != NULL, NULL);

    CHECK(*program->current_symbol == '(', NULL);
    ++program->current_symbol;
    if(*program->current_symbol == ')')
    {
        ++program->current_symbol;
        return createOp(OP_NOPARAM);
    }

    node_t* val = getN(program);
    skipSeparator(&program->current_symbol);

    if(*program->current_symbol == ',')
    {
        ++program->current_symbol;
        val = createNodeOp(OP_COMMA, val, createOp(OP_COMMA));
        getNodeParam(program, &val->right);
    }

    CHECK(*program->current_symbol == ')', NULL);
    ++program->current_symbol;

    return val;
}

//=========================================================================

void getNodeParam(program_t* program, node_t** node)
{
    CHECK(program != NULL, ;);
    CHECK(node    != NULL, ;);

    treeNodeDtor(*node);

    node_t* val = getN(program);
    CHECK(val != NULL, ;);
    *node = val;

    skipSeparator(&program->current_symbol);
    if(*program->current_symbol == ',')
    {
        ++program->current_symbol;
        *node = createNodeOp(OP_COMMA, val, createOp(OP_COMMA));
        getNodeParam(program, &(*node)->right);
    }
}

//=========================================================================

node_t* getRet(program_t* program)
{
    CHECK(program != NULL, NULL);

    node_t* ret = getE(program);
    ret = createNodeKey(KEY_RET, ret, createOp(OP_CONNECT));

    treeNodeDtor(ret->right);
    ret->right = NULL; 

    return ret;
}

//=========================================================================

node_t* callFunc(program_t* program)
{
    CHECK(program != NULL, NULL);

    node_t* val = NULL;
    name_t name = getName(program);
    CHECK(name != NULL, NULL);

    node_t* param = getParam(program);
    if(param == NULL)
    {
        return NULL;
    }
    if(param->data.opValue == OP_NOPARAM)
    {
        val = createNodeKey(KEY_FUNC, createOp(OP_CONNECT), createOp(OP_CONNECT));
        treeNodeDtor(val->left);
        val->left = NULL;
    }
    else
    {
        val = createNodeKey(KEY_FUNC, param, createOp(OP_CONNECT));
    }
    treeNodeDtor(val->right);
    val->right = NULL;
    val->name = name;

    return val;
}

//=========================================================================

node_t* getMain(program_t* program)
{
    CHECK(program != NULL, NULL);

    skipSeparator(&program->current_symbol);
    if(strncmp(program->current_symbol, "func", 4) != 0)
    {
        return NULL;
    }
    program->current_symbol += 4;

    node_t* val = getF(program);
    CHECK(val != NULL, NULL);
    CHECK(strcmp(val->name, "main") == 0, NULL);

    return val;    
}

//=========================================================================

node_t* getGlobal(program_t* program)
{
    CHECK(program != NULL, NULL);

    skipSeparator(&program->current_symbol);
    if(strncmp(program->current_symbol, "func", 4) == 0)
    {
        return NULL;
    }

    node_t* val = getN(program);
    
    skipSeparator(&program->current_symbol);
    if(strncmp(program->current_symbol, ":=", 2) != 0)
    {
        return NULL;
    }
    val = getAs(program, val->data.varValue);
    val = createNodeKey(KEY_DEC, val, createKey(KEY_DEC));

    skipSeparator(&program->current_symbol);
    if(strncmp(program->current_symbol, "func", 4) != 0)
    {
        getNodeGlobal(program, &val->right);
    }

    return val;          
}

//=========================================================================

node_t* getNodeGlobal(program_t* program, node_t** node)
{
    CHECK(program != NULL, NULL);
    CHECK(node    != NULL, NULL);

    skipSeparator(&program->current_symbol);
    if(strncmp(program->current_symbol, "func", 4) == 0)
    {
        return NULL;
    }

    treeNodeDtor(*node);

    node_t* val = getN(program);
    CHECK(val != NULL, NULL);
    skipSeparator(&program->current_symbol);

    val = getAs(program, val->data.varValue);
    val = createNodeKey(KEY_DEC, val, createKey(KEY_DEC));
    *node = val;

    skipSeparator(&program->current_symbol);
    if(strncmp(program->current_symbol, "func", 4) != 0)
    {
        getNodeGlobal(program, &(*node)->right);
    }
}

//=========================================================================

node_t* findStart(node_t* node)
{
    CHECK(node != NULL, NULL);

    if((node->type == KEY) && (node->data.keyValue == KEY_DEC))
    {
        if(node->right != NULL)
        {
            findStart(node->right);
        }
        else
        {
            return node;
        }
    }

}