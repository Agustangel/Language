#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <logger.h>
#include "lang.h"
#include "debug.h"


//=========================================================================

variable_t* varCtor(elem_t data, varName_t name)
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

    array->data = (arrayVar_t*) calloc(MAX_SIZE, sizeof(arrayVar_t));
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

int pushBack(arrayVar_t* array, elem_t data, varName_t name)
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
