#include <stdio.h>
#include <logger.h>
#include "lang.h"



int main()
{
    logger_init(1, "stack.log");
    logger_set_level(INFO);

    variable_t var_array[MAX_SIZE];

    logger_finalize(file);
    
    return 0;
}
