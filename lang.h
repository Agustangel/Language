#pragma once

#include <math.h>


#define MAX_SIZE 100
#define DATA_POISON NAN
#define NAME_POISON NULL
//=========================================================================

enum lang_codes
{
    LANG_ERROR                = -1,
    LANG_SUCCESS              =  0,
    ERR_LANG_NULL_PTR         =  1,
};

//=========================================================================

typedef double elem_t;
typedef const char* varName_t;

typedef struct variable
{
    elem_t data;
    varName_t name;
    
} variable_t;
