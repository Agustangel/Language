#pragma once

#include <math.h>
#include <stdint.h>
#include "tree.h"

#define DATA_POISON NAN
#define NAME_POISON NULL
//=========================================================================

enum lang_codes
{
    LANG_ERROR                = -1,
    LANG_SUCCESS              =  0,
    ERR_LANG_NULL_PTR         =  1,
    ERR_LANG_OUT_PLACE        =  2,
    ERR_LANG_NO_INPUT         =  3,
    ERR_LANG_NEGATIVE_COUNT   =  4,
};

enum
{
    NULL_INDEX  =  -1,
    START_INDEX =   0,
    MAX_SIZE    = 100,
};

typedef double elem_t;
typedef const char* name_t;

typedef struct variable
{
    elem_t value;
    name_t name;
    
} variable_t;

typedef struct arrayVar
{
    variable_t* data;
    int free_index;

} arrayVar_t;

//-------------------------------------------------------------------

typedef struct program
{
    char* buffer; 
    char* current_symbol;

} program_t;

//=========================================================================
variable_t* varCtor(elem_t data, name_t name);
int varDtor(variable_t* var);
int arrayVarCtor(arrayVar_t* array);
int arrayVarDtor(arrayVar_t* array);
int pushBack(arrayVar_t* array, elem_t data, name_t name);
int parseArgs(int argc, char* argv[], name_t* name_program);
int programCtor(FILE* file, program_t* program);
int programDtor(program_t* program);
size_t skipSeparator(char** string);
node_t* makeAST(program_t* program);
node_t* getS(program_t* program);
node_t* getE(program_t* program);
node_t* getT(program_t* program);
node_t* getN(program_t* program);
node_t* getP(program_t* program);
node_t* getL(program_t* program);
node_t* getIf(program_t* program);
node_t* getWhile(program_t* program);
node_t* getEx(program_t* program);
node_t* getAs(program_t* program, name_t name);
void getNodeS(program_t* program, node_t** node, char stop_symbol);
node_t* getF(program_t* program);
node_t* getParam(program_t* program);
void getNodeParam(program_t* program, node_t** node);
