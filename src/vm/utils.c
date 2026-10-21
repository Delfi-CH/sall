#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../../include/sall.h"
#include "../../include/vm.h"

bool is_register(int value, int registers[REGISTER_SIZE])
{
    if (value < 0 || value >= REGISTER_SIZE)
    {
        registers[ER] = NOREGISTER;
        return false;
    }
    else
    {
        return true;
    }
}

void print_instruction(int instruction[LINE_LENGTH]) {
    printf("Current Instruction:\n");
    for (int i = 0; i < LINE_LENGTH; i++) {
        printf("%d ", instruction[1]);
    }
    printf("\n");
}

void print_stack(int registers[REGISTER_SIZE], int stack[STACK_SIZE], int SP)
{
    printf("Stack Dump:\n");
    for (int i = 0; i < SP; i++)
    {
        printf("0x%04d ", stack[i]);
        if ((i + 1) % 4 == 0)
        {
            printf("\n");
        }
    }
    if (SP != 0)
    {
        printf("\n");
    }
}

void print_registers(int registers[REGISTER_SIZE])
{
    printf("Register Dump:\n");
    for (int i = 0; i < REGISTER_SIZE; i++)
    {
        printf("%04d ", registers[i]);
        if ((i + 1) % 4 == 0)
        {
            printf("\n");
        }
    }
    printf("\n");
}

void print_memory(int memory[MEM_SIZE])
{
    time_t unixtime = time(NULL);
    long int unixtime_int = (long int)unixtime;
    char filename[512];
    snprintf(filename, 512, "sall-memdump-%d", unixtime_int);
    FILE *file = fopen(filename, "w");
    printf("Writing Memory Dump to file %s\n", filename);
    for (int i = 0; i < MEM_SIZE; i++)
    {
        fprintf(file, "%04d ", memory[i]);
        if ((i + 1) % 4 == 0)
        {
            fprintf(file, "\n");
        }
    }
    fprintf(file, "\n");
    fclose(file);
}
