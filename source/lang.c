#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "lang.h"


int free_index = 0;
//=========================================================================

variable_t* varCtor(elem_t data, varName_t name)
{
    CHECK(name !=  NULL, NULL);

    variable_t* var = (variable_t*) calloc(1, sizeof(variable_t));
    CHECK(var !=  NULL, NULL);

    var->data = data;
    var->name = name;

    return var;
}

//=========================================================================

int varDtor(variable_t* var)
{
    CHECK(var !=  NULL, ERR_LANG_NULL_PTR);

    free(var);
    var->data = DATA_POISON;
    var->name = NAME_POISON;

    return LANG_SUCCESS;    
}

//=========================================================================

int pushBack(variable_t* var_array, variable_t* var)
{
    CHECK(var       !=  NULL, ERR_LANG_NULL_PTR);
    CHECK(var_array !=  NULL, ERR_LANG_NULL_PTR);

    LOG("In LINE %d, FUNCTION %s: free_index = %d\n", __LINE__, __PRETTY_FUNCTION__, free_index);
    if(free_index >= MAX_SIZE)
    {
        return ERR_LANG_OUT_PLACE;
    }
    
    var_array[free_index] = *var;
    ++free_index;

    return LANG_SUCCESS;
}

//=========================================================================
