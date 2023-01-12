#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lang.h"
#include "debug.h"
#include "tree.h"


//=========================================================================

variable_t* varCtor(int number, name_t name, int initialization, unsigned int hash)
{
    CHECK(name !=  NULL, NULL);

    variable_t* var = (variable_t*) calloc(1, sizeof(variable_t));
    CHECK(var !=  NULL, NULL);

    var->number = number;
    var->name   = name;
    var->hash           = hash;
    var->initialization = initialization;

    return var;
}

//=========================================================================

int varDtor(variable_t* var)
{
    CHECK(var !=  NULL, ERR_LANG_NULL_PTR);

    free(var);
    var->number = DATA_POISON;
    var->name   = NAME_POISON;
    var->hash           = DATA_POISON;
    var->initialization = DATA_POISON;

    return LANG_SUCCESS;    
}

//=========================================================================

variable_t* getVar(stack_t* vars, int idx) 
{
    CHECK(vars !=    NULL, NULL);
    CHECK(idx  >= 0, NULL);

    return ((variable_t*)((vars->data)[idx]));
}

//=========================================================================

variable_t* searchVar(stack_t* vars, char* name_var)
{
    CHECK(vars     != NULL, NULL);
    CHECK(name_var != NULL, NULL);

    unsigned int hash_var = hashFAQ6(name_var);
    for(int idx = 0; idx < vars->count; ++idx)
    {
        if(hash_var == getVar(vars, idx)->hash) 
        {
            return getVar(vars, idx);
        }
    }

    return NULL;
}

//=========================================================================

void freeVars(stack_t* vars)
{
    while(vars->count > 0)
    {
        variable_t* var = (variable_t*) stack_pop(vars);
        free(var->name);
        free(var);
    }
}

//=========================================================================

int RamCtor(RAM_t* ram)
{
    CHECK(ram !=  NULL, ERR_LANG_NULL_PTR);

    ram->locale_var = (stack_t*) calloc(1, sizeof(stack_t));
    ram->global_var = (stack_t*) calloc(1, sizeof(stack_t));

    CHECK(ram->locale_var !=  NULL, ERR_LANG_OUT_MEMORY);
    CHECK(ram->global_var !=  NULL, ERR_LANG_OUT_MEMORY);

    stack_init(ram->locale_var, INITIAL_CAPACITY);
    stack_init(ram->global_var, INITIAL_CAPACITY);

    return LANG_SUCCESS;
}

//=========================================================================

int RamDtor(RAM_t* ram)
{
    CHECK(ram !=  NULL, ERR_LANG_NULL_PTR);

    stack_destroy(ram->locale_var);
    stack_destroy(ram->global_var);

    free(ram->locale_var);
    free(ram->global_var);

    ram->locale_var = NULL;
    ram->global_var = NULL;

    return LANG_SUCCESS;
}

//=========================================================================

void generateAsmCode(tree_t* tree)
{
    CHECK(tree !=  NULL, ;);

    char name_output[MAX_SIZE];
    sprintf(name_output, "files/%s.asm", tree->name_equation);

    FILE* code = fopen(name_output, "w");

    if(!findMain(tree)) 
    {
        tree->status = ERR_LANG_NO_MAIN;
        return;
    }

    RAM_t ram;
    RamCtor(&ram);

    printBegin(tree, &ram, 0, code);
    // Print_dec(tree, &ram, code);

    // Free_vars(ram.global_var);

    fclose(code);
}

//=========================================================================

void printBegin(tree_t* tree, RAM_t* ram, int count_var, FILE* code)
{
    CHECK(tree != NULL, ;);
    CHECK(ram  != NULL, ;);
    CHECK(code != NULL, ;);

    printFindGlobalVar(tree, ram, code);

    fprintf(code, "\n\n"
                  "push %d\n"
                  "pop rfx\n"
                  "call :main\n"
                  "hlt\n\n\n", ram->global_var->count);

    node_t* node = tree->root;
    CHECK(node != NULL, ;);
    
    if((node->left != NULL) && (node->left->data.keyValue != KEY_ASSIGN))
    {
        printS(tree, node->left, ram, count_var, code);
    }
    if(node->right != NULL)
    {
        printS(tree, node->right, ram, count_var, code);
    }
}

//=========================================================================

void printS(tree_t* tree, node_t* node, RAM_t* ram, int count_var, FILE* code)
{
    CHECK(tree != NULL, ;);
    CHECK(node != NULL, ;);
    CHECK(ram  != NULL, ;);
    CHECK(code != NULL, ;);

    if((node->type == KEY) && (node->data.keyValue == KEY_FUNC))
    {
        printF(tree, node, ram, count_var, code);
    }
    if((node->type == KEY) && (node->data.keyValue == KEY_ASSIGN))
    {
        printAs(tree, node, ram, code);
    }
    if((node->type == KEY) && (node->data.keyValue == KEY_IF))
    {
        printIf(tree, node, ram, count_var, code);
    }
    if((node->type == KEY) && (node->data.keyValue == KEY_WHILE))
    {
        printWhile(tree, node, ram, count_var, code);
    }
    if((node->type == KEY) && (node->data.keyValue == KEY_RET))
    {
        printRet(tree, node, ram, code);
    }    
    if((node->type == OP) && (node->data.keyValue == OP_CONNECT))
    {
        if(node->left != NULL)
        {
            printS(tree, node->left, ram, count_var, code);
        }
        if(node->right != NULL)
        {
            printS(tree, node->right, ram, count_var, code);
        }        
    }
}

//=========================================================================

void printF(tree_t* tree, node_t* node, RAM_t* ram, int count_var, FILE* code)
{
    CHECK(tree != NULL, ;);
    CHECK(node != NULL, ;);
    CHECK(ram  != NULL, ;);
    CHECK(code != NULL, ;);

    findLocalVar(node->left, ram->locale_var, ram->global_var);
    int count_param = ram->locale_var->count;
    for(int idx = 0; idx < count_param; ++idx)
    {
        getVar(ram->locale_var, idx)->initialization = 1;
    }

    fprintf(code, "%s:\n", node->name);
    for(int idx = 0; idx < count_param; ++idx)
    {
        fprintf(code, "pop [rfx + %d]\n", idx);
    }
    fprintf(code, "\n");

    printBody(tree, node->right, ram, count_param, code);
    fprintf(code, "ret\n\n\n");

    freeVars(ram->locale_var);
}

//=========================================================================

void printBody(tree_t* tree, node_t* node, RAM_t* ram, int count_var, FILE* code)
{
    CHECK(tree != NULL, ;);
    CHECK(node != NULL, ;);
    CHECK(ram  != NULL, ;);
    CHECK(code != NULL, ;);

    if(node->left != NULL)
    {
        printS(tree, node->left, ram, count_var, code);
    }
    if(node->right != NULL)
    {
        printS(tree, node->right, ram, count_var, code);
    }
}

//=========================================================================

void printEquation(tree_t* tree, node_t* node, RAM_t* ram, FILE* code)
{
    CHECK(tree != NULL, ;);
    CHECK(node != NULL, ;);
    CHECK(ram  != NULL, ;);
    CHECK(code != NULL, ;);

    variable_t* var = NULL;
    switch(node->type)
    {
    case NUM: 
        fprintf(code, "push %d\n", node->data.intValue);
        break;

    case VAR:
        var = searchVar(ram->global_var, node->data.varValue);
        if(var != NULL)
        {
            fprintf(code, "push [%d]\n", var->number);
        }
        else 
        {
            variable_t* var = searchVar(ram->locale_var, node->name);
            if(var == NULL)
            {
                tree->status = ERR_LANG_NO_VAR;
                return;
            }
            if(var->initialization == 0)
            {
                tree->status = ERR_LANG_NO_INIT;
                return;
            }
            fprintf(code, "push [rfx + %d]\n", var->number);
        }
        break;

    case OP:
        if(node->left != NULL)
        {
            printEquation(tree, node->left,  ram, code);
        }
        if(node->right != NULL)
        {
            printEquation(tree, node->right, ram, code);
        }

        switch(node->data.opValue)
        {
        case OP_ADD:  fprintf(code, "add \n"); break;
        case OP_SUB:  fprintf(code, "sub \n"); break;
        case OP_MUL:  fprintf(code, "mul \n"); break;
        case OP_DIV:  fprintf(code, "div \n"); break;
        case OP_POW:  fprintf(code, "pow \n"); break;
        case OP_SIN:  fprintf(code, "sin \n"); break;
        case OP_COS:  fprintf(code, "cos \n"); break;
        case OP_EXP:  fprintf(code, "exp \n"); break;
        case OP_LN:   fprintf(code, "ln  \n"); break;
        case OP_SQRT: fprintf(code, "sqrt\n"); break;

        default:
            tree->status = ERR_LANG_BAD_SYNTAX;
            return;
        }
        break;
       
    default:
        tree->status = ERR_LANG_BAD_SYNTAX;
        return;
    }
}

//=========================================================================

void printIf(tree_t* tree, node_t* node, RAM_t* ram, int count_var, FILE* code)
{
    CHECK(tree != NULL, ;);
    CHECK(node != NULL, ;);
    CHECK(ram  != NULL, ;);
    CHECK(code != NULL, ;);

    size_t locale = ram->count_ifjmp;
    ++ram->count_ifjmp;

    fprintf(code, "\n"
                  "If_begin%lu:\n", locale);
    printEquation(tree, node->left->left,  ram, code);
    printEquation(tree, node->left->right, ram, code);

    if(node->left->type == OP)
    {
        switch(node->left->data.opValue)
        {
            case OP_MORE:    fprintf(code, "jbe :If_end%lu\n\n", locale); break;
            case OP_LESS:    fprintf(code, "jae :If_end%lu\n\n", locale); break;
            case OP_EQUAL:   fprintf(code, "jne :If_end%lu\n\n", locale); break;
            case OP_UNEQUAL: fprintf(code, "je  :If_end%lu\n\n", locale); break;
            
            default: break;
        }
    }
    else
    {
        tree->status = ERR_LANG_WRONG_CONDITION;
        return;
    }

    if((node->right->type == OP) && (node->right->data.opValue == OP_CONNECT))
    {
        printBody(tree, node->right, ram, count_var, code);
    }
    else
    {
        printS(tree, node->right, ram, count_var, code);
    }
    fprintf(code, "\n"
                  "If_end%lu:"
                  "\n\n", locale);
                  
}
//=========================================================================

void printWhile(tree_t* tree, node_t* node, RAM_t* ram, int count_var, FILE* code)
{
    CHECK(tree != NULL, ;);
    CHECK(node != NULL, ;);
    CHECK(ram  != NULL, ;);
    CHECK(code != NULL, ;);

    size_t locale = ram->count_ifjmp;
    ++ram->count_ifjmp;

    fprintf(code, "\n"
                  "While_begin%lu:\n", locale);
    printEquation(tree, node->left->left,  ram, code);
    printEquation(tree, node->left->right, ram, code);

    if(node->left->type == OP)
    {
        switch(node->left->data.opValue)
        {
            case OP_MORE:    fprintf(code, "jbe :While_end%lu\n\n", locale); break;
            case OP_LESS:    fprintf(code, "jae :While_end%lu\n\n", locale); break;
            case OP_EQUAL:   fprintf(code, "jne :While_end%lu\n\n", locale); break;
            //case OP_UNEQUAL: fprintf(code, "je  :While_end%lu\n\n", locale); break;
            
            default: break;
        }
    }
    else
    {
        tree->status = ERR_LANG_WRONG_CONDITION;
        return;
    }

    if((node->right->type == OP) && (node->right->data.opValue == OP_CONNECT))
    {
        printBody(tree, node->right, ram, count_var, code);
    }
    else
    {
        printS(tree, node->right, ram, count_var, code);
    }
    fprintf(code, "\n"
                  "jmp :While_begin%lu\n" 
                  "While_end%lu:\n\n", locale, locale);
                  
}

//=========================================================================

void printAs(tree_t* tree, node_t* node, RAM_t* ram, FILE* code)
{
    CHECK(tree != NULL, ;);
    CHECK(node != NULL, ;);
    CHECK(ram  != NULL, ;);
    CHECK(code != NULL, ;);

    variable_t* var = searchVar(ram->global_var, node->left->name);

    if(var != NULL)
    {
        printEquation(tree, node->right, ram, code);
        fprintf(code, "pop [%d]\n", var->number);
    }
    else 
    {
        variable_t* var = searchVar(ram->locale_var, node->left->data.varValue);
        if(var == NULL)
        {
            tree->status = ERR_LANG_NO_VAR;
            return;
        }
        
        var->initialization  = 1;

        printEquation(tree, node->right, ram, code);
        fprintf(code, "pop [rfx + %d]\n", var->number);
    }
}

//=========================================================================

void printArg(tree_t* tree, node_t* node, RAM_t* ram, FILE* code)
{
    CHECK(tree != NULL, ;);
    CHECK(node != NULL, ;);
    CHECK(ram  != NULL, ;);
    CHECK(code != NULL, ;);

    if((node->type == OP) && (node->data.opValue == OP_COMMA))
    {
        if(node->left  != NULL)
        {
            printArg(tree, node->left,  ram, code);
        }
        if(node->right != NULL)
        {
            printArg(tree, node->right, ram, code);
        }
    }
    else
    {  
        printEquation(tree, node, ram, code);
        fprintf(code, "\n");
    }
}

//=========================================================================

void printRet(tree_t* tree, node_t* node, RAM_t* ram, FILE* code)
{
    CHECK(tree != NULL, ;);
    CHECK(node != NULL, ;);
    CHECK(ram  != NULL, ;);
    CHECK(code != NULL, ;);

    printEquation(tree, node->left, ram, code);
    fprintf(code, "ret\n");
}

//=========================================================================

void printCall(tree_t* tree, node_t* node, RAM_t* ram, size_t count_var, FILE* code)
{
    CHECK(tree != NULL, ;);
    CHECK(node != NULL, ;);
    CHECK(ram  != NULL, ;);
    CHECK(code != NULL, ;);

    fprintf(code, "\n\n");
    if(node->left != NULL)
    {
        printArg(tree, node->left, ram, code);
    }

    fprintf(code, "\n"
                  "push %lu\n"
                  "push rfx\n"
                  "add\n"
                  "pop rfx\n"
                  "\n"
                  "call :%s\n"
                  "\n"
                  "push rfx\n"
                  "push %lu\n"
                  "sub\n"
                  "pop rfx\n"
                  "\n\n", count_var, node->name, count_var);
}

//=========================================================================

void printPrint(tree_t* tree, node_t* node, RAM_t* ram, FILE* code)
{
    CHECK(tree != NULL, ;);
    CHECK(node != NULL, ;);
    CHECK(ram  != NULL, ;);
    CHECK(code != NULL, ;);

    printEquation(tree, node->left, ram, code);
    fprintf(code, "out\n");
}

//=========================================================================

void printInput(tree_t* tree, node_t* node, RAM_t* ram, FILE* code)
{
    CHECK(tree != NULL, ;);
    CHECK(node != NULL, ;);
    CHECK(ram  != NULL, ;);
    CHECK(code != NULL, ;);

    variable_t* var = searchVar(ram->global_var, node->left->data.varValue);
    
    if(var != NULL)
    {
        fprintf(code, "in\n"
                      "pop [%d]\n", var->number);
    }
    else
    {
        var = searchVar(ram->locale_var, node->left->data.varValue);
        if(var == NULL)
        {
            tree->status = ERR_LANG_NO_VAR;
            return;
        }

        var->initialization = 1;

        fprintf(code, "in\n"
                      "pop [rfx + %d]\n", var->number);
    }
}

//=========================================================================

void printFindGlobalVar(tree_t* tree, RAM_t* ram, FILE* code)
{
    CHECK(tree != NULL, ;);
    CHECK(ram  != NULL, ;);
    CHECK(code != NULL, ;);

    node_t* node = tree->root;

    while((node->type == KEY) && (node->data.keyValue == KEY_DEC))
    {
        if(node->left != NULL)
        {
            if((node->left->type == KEY) && (node->left->data.keyValue == KEY_ASSIGN))
            {
                node_t* new_var = node->left->left;
                variable_t* var = searchVar(ram->global_var, new_var->name);

                if(var == NULL)
                {
                    var = varCtor(ram->global_var->count, new_var->name, 1, hashFAQ6(new_var->name));
                    stack_push(ram->global_var, var);
                }

                printEquation(tree, node->left->right, ram, code);
                fprintf(code, "pop [%d]\n", var->number);
            }
        }
        node = node->right;
        if(node == NULL) break;
    }

    if(node != NULL)
    {   
        if((node->type == KEY) && (node->data.keyValue == KEY_ASSIGN))
        {
            node_t* new_var = node->left;
            variable_t* var = searchVar(ram->global_var, new_var->data.varValue);

            if(var == NULL)
            {
                var = varCtor(ram->global_var->count, new_var->name, 1, hashFAQ6(new_var->name));
                stack_push(ram->global_var, var);
            }

            printEquation(tree, node->right, ram, code);
            fprintf(code, "pop [%d]\n", var->number);
        }
    }
}

//=========================================================================

void findLocalVar(node_t* node, stack_t* locale_vars, stack_t* global_vars)
{
    CHECK(node        != NULL, ;);
    CHECK(locale_vars != NULL, ;);
    CHECK(global_vars != NULL, ;);

    if(node->type == VAR)
    {
        if(!searchVar(global_vars, node->name) && !searchVar(locale_vars, node->name))
        {
            variable_t* new_var = varCtor(locale_vars->count, node->name, 0, hashFAQ6(node->name));
            stack_push(locale_vars, new_var);
        }
    }

    if(node->left  != NULL)
    {
        findLocalVar(node->left, locale_vars, global_vars);
    }
    if(node->right != NULL) 
    {
        findLocalVar(node->right, locale_vars, global_vars);
    }
}

//=========================================================================

int findMain(tree_t* tree)
{
    CHECK(tree != NULL, ERR_LANG_NULL_PTR);

    node_t* node = tree->root;

    while(node->right != NULL)
    {
        if(node->left != NULL)
        {
            if((node->left->data.keyValue == KEY_FUNC) && (strcmp(node->left->name, "main") == 0))
            {
                return 1;
            }
            else
            {
                node = node->right; 
            }
        }
        else
        {
            node = node->right;  
        }
    }

    if((node->left->data.keyValue == KEY_FUNC) && (strcmp(node->left->name, "main") == 0)) 
    {
        return 1;
    }

    return 0;
}

//=========================================================================

unsigned int hashFAQ6(char* cmd)
{
    CHECK(cmd != NULL, ERR_LANG_NULL_PTR);

    unsigned int hash = 0;
    
    for (int i = 0; i < strlen(cmd); ++i)
    {
        hash += (unsigned char)(*(cmd + i));
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}