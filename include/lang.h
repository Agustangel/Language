#pragma once

#include <math.h>
#include <stdint.h>


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

enum node_type
{
    NUM = 1,
    VAR = 2,
    OP  = 3
};

enum node_op
{
    OP_ERROR   = -1,
    OP_ADD     = 1,
    OP_SUB     = 2,
    OP_MUL     = 3,
    OP_DIV     = 4,
    OP_POW     = 5,
    OP_SIN     = 6,
    OP_COS     = 7,
    OP_EXP     = 8,
    OP_LN      = 9,
    OP_OPENBRT = 10,
    OP_CLOSBRT = 11
};

enum key_word
{
    KEY_IF     = 1,
    KEY_ELSE   = 2,
    KEY_WHILE  = 3,
};

//=========================================================================

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

//-------------------------------------------------------------------

typedef uint_fast16_t treeStatus_t;
typedef struct node_t node_t;

typedef struct treeData_t
{
    double        dblValue;
    const char*   varValue;
    enum node_op  opValue;
    enum key_word keyValue;

} treeData_t;

struct node_t
{
    enum node_type type;
    treeData_t     data;

    node_t* left;
    node_t* right;

};

typedef struct tree_t
{
    node_t* root;
    treeStatus_t status;

} tree_t;

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
node_t* getG();
node_t* getE();
node_t* getT();
node_t* getN();
node_t* getP();
node_t* getL();
 