#pragma once

#include <math.h>



#define DATA_POISON NAN
#define NAME_POISON NULL
//=========================================================================

enum lang_codes
{
    LANG_ERROR                = -1,
    LANG_SUCCESS              =  0,
    ERR_LANG_NULL_PTR         =  1,
    ERR_LANG_OUT_PLACE        =  2,
};

enum
{
    NULL_INDEX  =  -1,
    START_INDEX =   0,
    MAX_SIZE    = 100,
};

//=========================================================================

typedef double elem_t;
typedef const char* varName_t;

typedef struct variable
{
    elem_t value;
    varName_t name;
    
} variable_t;

typedef struct arrayVar
{
    variable_t* data;
    size_t free_index;

} arrayVar_t;

//=========================================================================
variable_t* varCtor(elem_t data, varName_t name);
int varDtor(variable_t* var);
int arrayVarCtor(arrayVar_t* array);
int arrayVarDtor(arrayVar_t* array);
int pushBack(variable_t* var_array, variable_t* var);
