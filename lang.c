#include <stdio.h>
#include <math.h>
#include "lang.h"



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