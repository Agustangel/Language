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
