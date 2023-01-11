#include <stdio.h>
#include <logger.h>
#include <onegin.h>
#include "lang.h"
#include "debug.h"


int main(int argc, char** argv)
{
    logger_init(1, "lang.log");
    logger_set_level(INFO);

    tree_t tree;
    treeCtor(&tree);

    name_t name_program = NAME_POISON;
    int ret = parseArgs(argc, argv, &name_program);
    CHECK(ret == LANG_SUCCESS, LANG_ERROR);

    FILE* text = fopen(name_program, "rb");
    CHECK(text != NULL, LANG_ERROR);

    program_t program;
    programCtor(text, &program);

    fclose(text);

    node_t* root = makeAST(&program);
    tree.root = root;
    dumpGraphTree(&tree);

    //generateAsmCode(&tree);

    programDtor(&program);
    treeDtor(&tree);
    
    logger_finalize(file);

    return 0;
}
