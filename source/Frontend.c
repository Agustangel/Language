#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <logger.h>
#include <onegin.h>
#include "lang.h"
#include "debug.h"



//=========================================================================

variable_t* varCtor(elem_t data, name_t name)
{
    CHECK(name !=  NULL, NULL);

    variable_t* var = (variable_t*) calloc(1, sizeof(variable_t));
    CHECK(var !=  NULL, NULL);

    var->value = data;
    var->name  = name;

    return var;
}

//=========================================================================

int varDtor(variable_t* var)
{
    CHECK(var !=  NULL, ERR_LANG_NULL_PTR);

    free(var);
    var->value = DATA_POISON;
    var->name  = NAME_POISON;

    return LANG_SUCCESS;    
}

//=========================================================================

int arrayVarCtor(arrayVar_t* array)
{
    CHECK(array !=  NULL, ERR_LANG_NULL_PTR);

    array->data = (variable_t*) calloc(MAX_SIZE, sizeof(variable_t));
    array->free_index = START_INDEX;

    return LANG_SUCCESS;
}

//=========================================================================

int arrayVarDtor(arrayVar_t* array)
{
    CHECK(array !=  NULL, ERR_LANG_NULL_PTR);

    free(array->data);
    array->data = NULL;
    array->free_index = NULL_INDEX;

    return LANG_SUCCESS;
}

//=========================================================================

int pushBack(arrayVar_t* array, elem_t data, name_t name)
{
    CHECK(name  !=  NULL, ERR_LANG_NULL_PTR);
    CHECK(array !=  NULL, ERR_LANG_NULL_PTR);

    LOG("In LINE %d, FUNCTION %s: free_index = %d\n", __LINE__, __PRETTY_FUNCTION__, array->free_index);
    if(array->free_index >= MAX_SIZE)
    {
        return ERR_LANG_OUT_PLACE;
    }
    
    variable_t* var = varCtor(data, name);
    array->data[array->free_index] = *var;
    ++array->free_index;

    return LANG_SUCCESS;
}

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
    while(isspace((int)(unsigned char)*string))
    {
        ++(string);
        ++count;
    }

    return count; 
}

//=========================================================================

node_t* makeAST(program_t* program)
{
    CHECK(program != NULL, NULL);

    node_t* root = NULL;
    if(*program->current_symbol != '\0')
    {
        skipSeparator(&program->current_symbol);
        root = getG();
        CHECK(root != NULL, NULL);
    }
    
    return root;
}

//-------------------------------------------------------------------

node_t* getG(program_t* program)
{
    node_t* val = getE();
    CHECK(*program->current_symbol == '\0', NULL);

    return val;
}

//-------------------------------------------------------------------

node_t* getN(program_t* program)
{
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

        while((*program->current_symbol >= '0') && (*program->current_symbol <= '9'))
        {
            val = (val * 10) + (*program->current_symbol - '0');
            ++program->current_symbol;
        }
        val *= tmp;
        CHECK(s > sOld, NULL);

        return createNum(val);
    }
    else
    {
        const char* elem = program->current_symbol;
        ++program->current_symbol;

        if(tmp == -1)
        {
            return Mul(createVar(elem), createNum(-1));
        }
        return createVar(elem);
    }
}

//-------------------------------------------------------------------

node_t* getE(program_t* program)
{
    skipSeparator(&program->current_symbol);
    node_t* val = getT();

    skipSeparator(&program->current_symbol);
    while((*program->current_symbol == '+') || (*program->current_symbol == '-'))
    {
        char op = *program->current_symbol;
        ++program->current_symbol;
        skipSeparator(&program->current_symbol);

        node_t* val_2 = getT();
        if(op == '+')
        {
            val = Add(val, val_2);
        }
        else
        {
            val = Sub(val, val_2);
        }
    }

    return val;
}

//-------------------------------------------------------------------

node_t* getT(program_t* program)
{
    skipSeparator(&program->current_symbol);
    node_t* val = getL();

    skipSeparator(&program->current_symbol);
    while((*program->current_symbol == '*') || (*program->current_symbol == '/'))
    {
        char op = *program->current_symbol;
        ++program->current_symbol;
        skipSeparator(&program->current_symbol);

        node_t* val_2 = getL();
        if(op == '*')
        {
            val = Mul(val, val_2);
        }
        else
        {
            val = Div(val, val_2);
        }
    }

    return val;    
}

//-------------------------------------------------------------------

node_t* getP(program_t* program)
{
    skipSeparator(&program->current_symbol);
    node_t* val = 0;
    
    if(*program->current_symbol == '(')
    {
        ++program->current_symbol;
        skipSeparator(&program->current_symbol);
        val = getE();
        skipSeparator(&program->current_symbol);

        CHECK(*program->current_symbol == ')', NULL);
        ++program->current_symbol;

        return Bracket(val);
    }
    else
    {
        val = getN();
    }

    return val;
}

//-------------------------------------------------------------------

node_t* getL(program_t* program)
{
    skipSeparator(&program->current_symbol);
    if(strncmp(program->current_symbol, "cos", 3) == 0)
    {
        program->current_symbol += 3;
        return Cos(getP());
    }
    if(strncmp(program->current_symbol, "sin", 3) == 0)
    {
        program->current_symbol += 3;
        return Sin(getP());
    }
    if(strncmp(program->current_symbol, "exp", 3) == 0)
    {
        program->current_symbol += 3;
        return Exp(getP());        
    }
    if(strncmp(program->current_symbol, "ln", 2) == 0)
    {
        program->current_symbol += 2;
        return Ln(getP());        
    }

    node_t* val = getP();
    skipSeparator(&program->current_symbol);
    if(*program->current_symbol == '^')
    {
        ++program->current_symbol;
        skipSeparator(&program->current_symbol);
        node_t* power = getP();
        skipSeparator(&program->current_symbol);
        val = Pow(val, power);
    }
    CHECK(*s != '^', NULL);

    return val;
}
