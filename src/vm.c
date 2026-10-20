#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/tinyvm.h"

#define STACK_SIZE 256
#define MEM_SIZE 1024
#define MAX_PROGRAM_LENGTH 1024
#define LINE_LENGTH 5

static int stack[STACK_SIZE];
static int memory[MEM_SIZE];

static int registers[REGISTER_SIZE];

static int program[MAX_PROGRAM_LENGTH][LINE_LENGTH];
static int program_line_lenghts[MAX_PROGRAM_LENGTH];
static int program_length = 0;

/** if the program is running */
static bool running = true;

/** if the IP is assigned by jmp instructions(such as IF,IFN),it should not increase 1 any more **/
bool is_jmp = false;

/** quick ways to get SP and IP */
#define SP (registers[SP])
#define PC (registers[PC])

/** prints the stack from A to B */
void print_stack()
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

void print_registers()
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

void print_memory()
{
    time_t unixtime = time(NULL);
    long int unixtime_int = (long int)unixtime;
    char filename[512];
    snprintf(filename, 512, "tvm-memdump-%d", unixtime_int);
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

bool is_register(int value)
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

void eval(int instruction[LINE_LENGTH])
{
    is_jmp = false;
    registers[ER] = NOERR;
    switch (instruction[0])
    {
    case HLT:
    {
        if (program_line_lenghts[PC] < 1)
        {
            registers[ER] = INVALID;
            break;
        }
        else
        {
            running = false;
            printf("done\n");
            break;
        }
    }
    case LDR:
    {
        if (program_line_lenghts[PC] < 3)
        {
            registers[ER] = INVALID;
            break;
        }
        else
        {
            if (!is_register(instruction[1]))
            {
                break;
            }
            registers[instruction[1]] = instruction[2];
            break;
        }
    }
    case TRR:
    {
        if (program_line_lenghts[PC] < 3)
        {
            registers[ER] = INVALID;
            break;
        }
        else
        {
            if (!is_register(instruction[1]))
            {
                break;
            }
            if (!is_register(instruction[2]))
            {
                break;
            }
            registers[instruction[2]] = registers[instruction[1]];
            break;
        }
    }
    case PSH:
    {
        if (program_line_lenghts[PC] < 2)
        {
            registers[ER] = INVALID;
            break;
        }
        else
        {
            SP++;
            if (SP > STACK_SIZE)
            {
                registers[ER] = STOVERFLOW;
                break;
            }
            stack[SP] = instruction[1];
            break;
        }
    }
    case PSR:
    {
        if (program_line_lenghts[PC] < 2)
        {
            registers[ER] = INVALID;
            break;
        }
        else
        {
            SP++;
            if (SP > STACK_SIZE)
            {
                registers[ER] = STOVERFLOW;
                break;
            }
            if (!is_register(instruction[1]))
            {
                break;
            }
            stack[SP] = registers[instruction[1]];
            break;
        }
    }
    case POP:
    {
        if (program_line_lenghts[PC] < 1)
        {
            registers[ER] = INVALID;
            break;
        }
        else
        {
            SP--;
            if (SP < 0)
            {
                registers[ER] = STUNDERFLOW;
            }
            break;
        }
    }
    case PLR:
    {
        if (program_line_lenghts[PC] < 2)
        {
            registers[ER] = INVALID;
            break;
        }
        else
        {
            int curr_sp = SP - 1;
            if (curr_sp < 0)
            {
                registers[ER] = STUNDERFLOW;
            }
            if (!is_register(instruction[1]))
            {
                break;
            }
            registers[instruction[1]] = stack[SP];
            SP--;
            break;
        }
    }
    case SLR:
    {
        if (program_line_lenghts[PC] < 2)
        {
            registers[ER] = INVALID;
            break;
        }
        else
        {
            if (!is_register(instruction[1]))
            {
                break;
            }
            registers[instruction[1]] = stack[SP];
            break;
        }
    }
    case ADD:
    {
        if (program_line_lenghts[PC] < 2)
        {
            registers[ER] = INVALID;
            break;
        }
        else
        {
            if (!is_register(instruction[1]))
            {
                break;
            }
            registers[instruction[1]] = registers[instruction[1]] + registers[A];
            break;
        }
    }
    case SUB:
    {
        if (program_line_lenghts[PC] < 2)
        {
            registers[ER] = INVALID;
            break;
        }
        else
        {
            if (!is_register(instruction[1]))
            {
                break;
            }
            registers[instruction[1]] = registers[instruction[1]] - registers[A];
            break;
        }
    }
    case MUL:
    {
        if (program_line_lenghts[PC] < 2)
        {
            registers[ER] = INVALID;
            break;
        }
        else
        {
            if (!is_register(instruction[1]))
            {
                break;
            }
            registers[instruction[1]] = registers[instruction[1]] * registers[A];
            break;
        }
    }
    case DIV:
    {
        if (program_line_lenghts[PC] < 2)
        {
            registers[ER] = INVALID;
            break;
        }
        else
        {
            if (!is_register(instruction[1]))
            {
                break;
            }
            if (registers[A] == 0)
            {
                registers[ER] = DIVZERO;
                break;
            }
            registers[instruction[1]] = registers[instruction[1]] / registers[A];
            break;
        }
    }
    case JMP:
    {
        if (program_line_lenghts[PC] < 2)
        {
            registers[ER] = INVALID;
            break;
        }
        else
        {
            if (instruction[1] > program_length)
            {
                registers[ER] = PCOOB;
                break;
            }
            is_jmp = true;
            PC = instruction[1];
            break;
        }
    }
    case INC:
    {
        if (program_line_lenghts[PC] < 2)
        {
            registers[ER] = INVALID;
            break;
        }
        else
        {
            if (instruction[1] + PC > program_length)
            {
                registers[ER] = PCOOB;
                break;
            }
            is_jmp = true;
            PC = PC + instruction[1];
            break;
        }
    }
    case IFR:
    {
        if (program_line_lenghts[PC] < 4)
        {
            registers[ER] = INVALID;
            break;
        }
        else
        {
            if (!is_register(instruction[1]))
            {
                break;
            }
            if (instruction[3] > program_length)
            {
                registers[ER] = PCOOB;
                break;
            }
            if (registers[instruction[1]] == instruction[2])
            {
                PC = instruction[3];
            }
            break;
        }
    }
    case IRR:
    {
        if (program_line_lenghts[PC] < 4)
        {
            registers[ER] = INVALID;
            break;
        }
        else
        {
            if (!is_register(instruction[1]))
            {
                break;
            }
            if (!is_register(instruction[2]))
            {
                break;
            }
            if (instruction[3] > program_length)
            {
                registers[ER] = PCOOB;
                break;
            }
            if (registers[instruction[1]] == registers[instruction[2]])
            {
                PC = instruction[3];
            }
            break;
        }
    }
    case INR:
    {
        if (program_line_lenghts[PC] < 4)
        {
            registers[ER] = INVALID;
            break;
        }
        else
        {
            if (!is_register(instruction[1]))
            {
                break;
            }
            if (instruction[3] > program_length)
            {
                registers[ER] = PCOOB;
                break;
            }
            if (registers[instruction[1]] != instruction[2])
            {
                PC = instruction[3];
            }
            break;
        }
    }
    case INN:
    {
        if (program_line_lenghts[PC] < 4)
        {
            registers[ER] = INVALID;
            break;
        }
        else
        {
            if (!is_register(instruction[1]))
            {
                break;
            }
            if (!is_register(instruction[2]))
            {
                break;
            }
            if (instruction[3] > program_length)
            {
                registers[ER] = PCOOB;
                break;
            }
            if (registers[instruction[1]] != registers[instruction[2]])
            {
                PC = instruction[3];
            }
            break;
        }
    }
    case PRT:
    {
        if (program_line_lenghts[PC] < 2)
        {
            registers[ER] = INVALID;
            break;
        }
        else
        {
            if (!is_register(instruction[1]))
            {
                break;
            }
            printf("%d\n", registers[instruction[1]]);
            break;
        }
    }
    case NOP:
    {
        if (program_line_lenghts[PC] < 1)
        {
            registers[ER] = INVALID;
            break;
        }
        else
        {
            break;
        }
    }
    case JMR:
    {
        if (program_line_lenghts[PC] < 2)
        {
            registers[ER] = INVALID;
            break;
        }
        else
        {
            if (!is_register(instruction[1]))
            {
                break;
            }
            if (registers[instruction[1]] > program_length)
            {
                registers[ER] = PCOOB;
                break;
            }
            is_jmp = true;
            PC = registers[instruction[1]];
            break;
        }
    }
    case ICR:
    {
        if (program_line_lenghts[PC] < 2)
        {
            registers[ER] = INVALID;
            break;
        }
        else
        {
            if (!is_register(instruction[1]))
            {
                break;
            }
            if (registers[instruction[1]] + PC > program_length)
            {
                registers[ER] = PCOOB;
                break;
            }
            is_jmp = true;
            PC = PC + registers[instruction[1]];
            break;
        }
    }
    default:
    {
        registers[ER] = INVALID;
        break;
    }
    }

    switch (registers[ER])
    {
    case NOERR:
    {
        break;
    }
    case INVALID:
    {
        running = false;
        printf("error: invalid instruction at %d, bad instruction or missing operands\n", PC);
        print_stack();
        print_registers();
        print_memory();
        break;
    }
    case NOREGISTER:
    {
        running = false;
        printf("error: invalid instruction at %d, register %d doesnt exist\n", PC, instruction[1]);
        print_stack();
        print_registers();
        print_memory();
        break;
    }
    case STOVERFLOW:
    {
        running = false;
        printf("error: stack overflow at %d\n", PC);
        print_stack();
        print_registers();
        print_memory();
        break;
    }
    case STUNDERFLOW:
    {
        running = false;
        printf("error: stack undeflow at %d\n", PC);
        print_stack();
        print_registers();
        print_memory();
        break;
    }
    case DIVZERO:
    {
        running = false;
        printf("error: divide by zero at %d\n", PC);
        print_stack();
        print_registers();
        print_memory();
        break;
    }
    case PCOOB:
    {
        running = false;
        printf("error: program counter out of bounds\n");
        print_stack();
        print_registers();
        print_memory();
        break;
    }
    default:
    {
        running = false;
        printf("error: unexpected error\n");
        break;
    }
    }
}

int main(int argc, char **argv)
{
    if (argc <= 1)
    {
        printf("error: no input files\n");
        return -1;
    }

    char *filename = argv[1];

    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("error: could not read file `%s`\n", filename);
        return -1;
    }

    char line[512];

    while (fgets(line, sizeof line, file) && program_length < MAX_PROGRAM_LENGTH)
    {
        int arr[LINE_LENGTH];
        int index = 0;

        char *token = strtok(line, " \t\n");

        while (token != NULL && index < LINE_LENGTH)
        {
            program[program_length][index++] = atoi(token);
            token = strtok(NULL, " \t\n");
        }

        program_line_lenghts[program_length] = index;
        program_length++;
    }

    fclose(file);

    SP = -1;

    while (running && PC < program_length)
    {
        eval(program[PC]);
        if (!is_jmp)
        {
            PC++;
        }
    }

    return 0;
}