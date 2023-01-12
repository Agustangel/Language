#pragma once

#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stack.h>
#include <logger.h>
#include <onegin.h>

#include "tree.h"


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
    ERR_LANG_BAD_NAME         =  5,
    ERR_LANG_OUT_MEMORY       =  6,
    ERR_LANG_NO_MAIN          =  7,
    ERR_LANG_NO_VAR           =  8,
    ERR_LANG_NO_INIT          =  9,
    ERR_LANG_BAD_SYNTAX       =  10,
    ERR_LANG_WRONG_CONDITION  =  11
};

enum
{
    DATA_POISON      = -11,
    NULL_INDEX       =  -1,
    START_INDEX      =   2,
    MAX_SIZE         = 100,
    INITIAL_CAPACITY =  20
};

//=========================================================================

typedef struct RAM
{
    size_t bp;

    size_t count_ifjmp;

    stack_t* locale_var;
    stack_t* global_var;

} RAM_t;

//-------------------------------------------------------------------

typedef struct variable
{
    int number;
    name_t name;

    unsigned int hash;
    int initialization;

} variable_t;

//-------------------------------------------------------------------

typedef struct program
{
    char* buffer; 
    char* current_symbol;

} program_t;

//=========================================================================

int parseArgs(int argc, char* argv[], name_t* name_program);
int programCtor(FILE* file, program_t* program);
int programDtor(program_t* program);
size_t skipSeparator(char** string);
node_t* makeAST(program_t* program);
void getNodeDec(program_t* program, node_t** node, char stop_symbol);
node_t* getS(program_t* program);
node_t* getE(program_t* program);
node_t* getT(program_t* program);
node_t* getN(program_t* program);
node_t* getP(program_t* program);
node_t* getL(program_t* program);
node_t* getIf(program_t* program);
node_t* getWhile(program_t* program);
node_t* getRet(program_t* program);
node_t* getAs(program_t* program, name_t name);
void getNodeS(program_t* program, node_t** node, char stop_symbol);
node_t* getF(program_t* program);
node_t* getParam(program_t* program);
void getNodeParam(program_t* program, node_t** node);
char* getName(program_t* program);
int getLenName(program_t* program);
node_t* callFunc(program_t* program);
node_t* getMain(program_t* program);
node_t* getGlobal(program_t* program);
node_t* getNodeGlobal(program_t* program, node_t** node);
node_t* findStart(node_t* node);

//-------------------------------------------------------------------

variable_t* getVar(stack_t* vars, int idx);
void freeVars(stack_t* vars);
variable_t* searchVar(stack_t* vars, char* name_var);
variable_t* varCtor(int number, name_t name, int initialization, unsigned int hash);
int varDtor(variable_t* var);
int RamCtor(RAM_t* ram);
int RamDtor(RAM_t* ram);
void generateAsmCode(tree_t* tree);
int findMain(tree_t* tree);
void printBegin(tree_t* tree, RAM_t* ram, FILE* code);
void printDec(tree_t* tree, RAM_t* ram, FILE* code);
void printF(tree_t* tree, node_t* node, RAM_t* ram, FILE* code);
void printS(tree_t* tree, node_t* node, RAM_t* ram, int count_var, FILE* code);
void printBody(tree_t* tree, node_t* node, RAM_t* ram, int count_var, FILE* code);
void printAs(tree_t* tree, node_t* node, RAM_t* ram, FILE* code);
void printEquation(tree_t* tree, node_t* node, RAM_t* ram, FILE* code);
void printIf(tree_t* tree, node_t* node, RAM_t* ram, int count_var, FILE* code);
void printWhile(tree_t* tree, node_t* node, RAM_t* ram, int count_var, FILE* code);
void findLocalVar(node_t* node, stack_t* locale_vars, stack_t* global_vars);
void printFindGlobalVar(tree_t* tree, RAM_t* ram, FILE* code);
unsigned int hashFAQ6(char* cmd);
void printArg(tree_t* tree, node_t* node, RAM_t* ram, FILE* code);
void printRet(tree_t* tree, node_t* node, RAM_t* ram, FILE* code);
void printCall(tree_t* tree, node_t* node, RAM_t* ram, size_t count_var, FILE* code);
void printPrint(tree_t* tree, node_t* node, RAM_t* ram, FILE* code);
void printInput(tree_t* tree, node_t* node, RAM_t* ram, FILE* code);
