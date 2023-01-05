#include <stdio.h>
#include <logger.h>
#include <onegin.h>
#include "lang.h"



int main(int argc, char** argv)
{
    logger_init(1, "stack.log");
    logger_set_level(INFO);

    tree_t tree;
    treeCtor(&tree);

    arrayVar_t array[MAX_SIZE];
    arrayVarCtor(array);

    name_t name_program = NAME_POISON;
    int ret = parseArgs(argc, argv, &name_program);
    CHECK(ret == LANG_SUCCESS, LANG_ERROR);

    FILE* file = fopen(name_program, "rb");
    CHECK(file != ERR_LANG_NULL_PTR, LANG_ERROR);

    program_t program;
    programCtor(file, &program);

    fclose(file);

    node_t* root = makeAST(&program);
    tree.root = root;

    programDtor(&program);
    arrayVarDtor(array);
    
    logger_finalize(file);

    return 0;
}
