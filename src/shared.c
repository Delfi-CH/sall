#include "../include/sall.h"
#include <string.h>

InstructionSet str2ins(const char *str)
{
    int j;
    for (j = 0; j < sizeof(instruction_conversion) / sizeof(instruction_conversion[0]); ++j)
        if (!strcmp(str, instruction_conversion[j].str))
            return instruction_conversion[j].instruction;
    return -1;
}

Registers str2reg(const char *str)
{
    int j;
    for (j = 0; j < sizeof(register_conversion) / sizeof(register_conversion[0]); ++j)
        if (!strcmp(str, register_conversion[j].str))
            return register_conversion[j].reg;
    return -1;
}

ErrorCodes str2err(const char *str)
{
    int j;
    for (j = 0; j < sizeof(error_conversion) / sizeof(error_conversion[0]); ++j)
        if (!strcmp(str, error_conversion[j].str))
            return error_conversion[j].error;
    return -1;
}

InterruptMask str2interrupt_mask(const char *str) {
    int j;
    for (j = 0; j < sizeof(interrupt_mask_conversion) / sizeof(interrupt_mask_conversion[0]); ++j)
        if (!strcmp(str, interrupt_mask_conversion[j].str))
            return interrupt_mask_conversion[j].mask;
    return -1;
}

PrintMode str2prtint_mode(const char *str) {
    int j;
    for (j = 0; j < sizeof(print_mode_conversion) / sizeof(print_mode_conversion[0]); ++j)
        if (!strcmp(str, print_mode_conversion[j].str))
            return print_mode_conversion[j].mode;
    return -1;
}