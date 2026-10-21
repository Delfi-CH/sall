#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include "../../include/sall.h"
#include "../../include/vm.h"

int stack[STACK_SIZE];
int memory[MEM_SIZE];
int registers[REGISTER_SIZE];

static Program prog;

static bool running = true;

/** if the PC is assigned by jmp instructions,it should not increase 1 any more **/
bool is_jmp = false;

bool interrupted = false;

/** quick ways to get SP and PC */
#define SP (registers[SP])
#define PC (registers[PC])

void prt(int value, PrintMode mode)
{
    switch (mode)
    {
    case DECIMAL:
    {
        printf("%d", value);
        break;
    }
    case HEX:
    {
        printf("0x%x", value);
        break;
    }
    case CHAR:
    {
        printf("%c", value);
        break;
    }
    }
}

void check_err(int instruction[LINE_LENGTH])
{
    bool panic = false;
    switch (registers[ER])
    {
    case NOERR:
    {
        break;
    }
    case INVALID:
    {
        panic = true;
        printf("error: invalid instruction at %d, bad instruction or missing operands\n", PC);
        break;
    }
    case NOREGISTER:
    {
        panic = true;
        printf("error: invalid instruction at %d, register %d doesnt exist\n", PC, instruction[1]);
        break;
    }
    case STOVERFLOW:
    {
        panic = true;
        printf("error: stack overflow at %d\n", PC);
        break;
    }
    case STUNDERFLOW:
    {
        panic = true;
        printf("error: stack undeflow at %d\n", PC);
        break;
    }
    case DIVZERO:
    {
        panic = true;
        printf("error: divide by zero at %d\n", PC);
        break;
    }
    case PCOOB:
    {
        panic = true;
        printf("error: program counter out of bounds\n");
        break;
    }
    default:
    {
        panic = true;
        printf("error: unexpected error\n");
        break;
    }
    }

    if (panic)
    {
        running = false;
        print_instruction(instruction);
        print_stack(registers, stack, SP);
        print_registers(registers);
        print_memory(memory);
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
        if (prog.line_lengths[PC] < 1)
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
        if (prog.line_lengths[PC] < 3)
        {
            registers[ER] = INVALID;
            break;
        }
        else
        {
            if (!is_register(instruction[1], registers))
            {
                break;
            }
            registers[instruction[1]] = instruction[2];
            break;
        }
    }
    case TRR:
    {
        if (prog.line_lengths[PC] < 3)
        {
            registers[ER] = INVALID;
            break;
        }
        else
        {
            if (!is_register(instruction[1], registers))
            {
                break;
            }
            if (!is_register(instruction[2], registers))
            {
                break;
            }
            registers[instruction[2]] = registers[instruction[1]];
            break;
        }
    }
    case PSH:
    {
        if (prog.line_lengths[PC] < 2)
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
        if (prog.line_lengths[PC] < 2)
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
            if (!is_register(instruction[1], registers))
            {
                break;
            }
            stack[SP] = registers[instruction[1]];
            break;
        }
    }
    case POP:
    {
        if (prog.line_lengths[PC] < 1)
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
        if (prog.line_lengths[PC] < 2)
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
            if (!is_register(instruction[1], registers))
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
        if (prog.line_lengths[PC] < 2)
        {
            registers[ER] = INVALID;
            break;
        }
        else
        {
            if (!is_register(instruction[1], registers))
            {
                break;
            }
            registers[instruction[1]] = stack[SP];
            break;
        }
    }
    case ADD:
    {
        if (prog.line_lengths[PC] < 2)
        {
            registers[ER] = INVALID;
            break;
        }
        else
        {
            if (!is_register(instruction[1], registers))
            {
                break;
            }
            registers[instruction[1]] = registers[instruction[1]] + registers[A];
            break;
        }
    }
    case SUB:
    {
        if (prog.line_lengths[PC] < 2)
        {
            registers[ER] = INVALID;
            break;
        }
        else
        {
            if (!is_register(instruction[1], registers))
            {
                break;
            }
            registers[instruction[1]] = registers[instruction[1]] - registers[A];
            break;
        }
    }
    case MUL:
    {
        if (prog.line_lengths[PC] < 2)
        {
            registers[ER] = INVALID;
            break;
        }
        else
        {
            if (!is_register(instruction[1], registers))
            {
                break;
            }
            registers[instruction[1]] = registers[instruction[1]] * registers[A];
            break;
        }
    }
    case DIV:
    {
        if (prog.line_lengths[PC] < 2)
        {
            registers[ER] = INVALID;
            break;
        }
        else
        {
            if (!is_register(instruction[1], registers))
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
        if (prog.line_lengths[PC] < 2)
        {
            registers[ER] = INVALID;
            break;
        }
        else
        {
            if (instruction[1] > prog.length)
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
        if (prog.line_lengths[PC] < 2)
        {
            registers[ER] = INVALID;
            break;
        }
        else
        {
            if (instruction[1] + PC > prog.length)
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
        if (prog.line_lengths[PC] < 5)
        {
            registers[ER] = INVALID;
            break;
        }
        else
        {
            if (!is_register(instruction[1], registers))
            {
                break;
            }
            if (instruction[3] > prog.length)
            {
                registers[ER] = PCOOB;
                break;
            }
            if (registers[instruction[1]] == instruction[2])
            {
                PC = PC + instruction[3];
            }
            else
            {
                PC = PC + instruction[4];
            }
            break;
        }
    }
    case IRR:
    {
        if (prog.line_lengths[PC] < 5)
        {
            registers[ER] = INVALID;
            break;
        }
        else
        {
            if (!is_register(instruction[1], registers))
            {
                break;
            }
            if (!is_register(instruction[2], registers))
            {
                break;
            }
            if (instruction[3] > prog.length)
            {
                registers[ER] = PCOOB;
                break;
            }
            if (registers[instruction[1]] == registers[instruction[2]])
            {
                PC = PC + instruction[3];
            }
            else
            {
                PC = PC + instruction[4];
            }
            break;
        }
    }
    case INR:
    {
        if (prog.line_lengths[PC] < 5)
        {
            registers[ER] = INVALID;
            break;
        }
        else
        {
            if (!is_register(instruction[1], registers))
            {
                break;
            }
            if (instruction[3] > prog.length)
            {
                registers[ER] = PCOOB;
                break;
            }
            if (registers[instruction[1]] != instruction[2])
            {
                PC = PC + instruction[3];
            }
            else
            {
                PC = PC + instruction[4];
            }
            break;
        }
    }
    case INN:
    {
        if (prog.line_lengths[PC] < 5)
        {
            registers[ER] = INVALID;
            break;
        }
        else
        {
            if (!is_register(instruction[1], registers))
            {
                break;
            }
            if (!is_register(instruction[2], registers))
            {
                break;
            }
            if (instruction[3] > prog.length)
            {
                registers[ER] = PCOOB;
                break;
            }
            if (registers[instruction[1]] != registers[instruction[2]])
            {
                PC = PC + instruction[3];
            }
            else
            {
                PC = PC + instruction[4];
            }
            break;
        }
    }
    case PRT:
    {
        if (prog.line_lengths[PC] < 3)
        {
            registers[ER] = INVALID;
            break;
        }
        else
        {
            if (!is_register(instruction[1], registers))
            {
                break;
            }
            if (instruction[2] < 0 || instruction[2] > 2)
            {
                break;
            }
            pid_t pid;
            if (registers[IM] == 2 || registers[IM] == 3 || registers[IM] == 6 || registers[IM] == 7 )
            {
                pid = fork();
            }
            else
            {
                pid = vfork();
            }

            if (pid == 0)
            {
                prt(registers[instruction[1]], instruction[2]);
                _exit(0);
            }
            break;
        }
    }
    case NOP:
    {
        if (prog.line_lengths[PC] < 1)
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
        if (prog.line_lengths[PC] < 2)
        {
            registers[ER] = INVALID;
            break;
        }
        else
        {
            if (!is_register(instruction[1], registers))
            {
                break;
            }
            if (registers[instruction[1]] > prog.length)
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
        if (prog.line_lengths[PC] < 2)
        {
            registers[ER] = INVALID;
            break;
        }
        else
        {
            if (!is_register(instruction[1], registers))
            {
                break;
            }
            if (registers[instruction[1]] + PC > prog.length)
            {
                registers[ER] = PCOOB;
                break;
            }
            is_jmp = true;
            PC = PC + registers[instruction[1]];
            break;
        }
    }
    case SLP: {
        if (prog.line_lengths[PC] < 2)
        {
            registers[ER] = INVALID;
            break;
        }
        else {
            struct timespec dur = {0};
            struct timespec remaining;
            dur.tv_sec = instruction[1] / 1000;
            dur.tv_nsec = (instruction[1] % 1000) * 1000000L;
            nanosleep(&dur, &remaining);
            break;
        }
    }
    case SPR: {
        if (prog.line_lengths[PC] < 2)
        {
            registers[ER] = INVALID;
            break;
        }
        else {
            if (!is_register(instruction[1], registers))
            {
                break;
            }
            struct timespec dur = {0};
            struct timespec remaining;
            dur.tv_sec = instruction[1] / 1000;
            dur.tv_nsec = (instruction[1] % 1000) * 1000000L;
            nanosleep(&dur, &remaining);
            break;
        }
    }
    case IKB: {
        if (prog.line_lengths[PC] < 2)
        {
            registers[ER] = INVALID;
            break;
        }
        else {
            if (!is_register(instruction[1], registers))
            {
                break;
            }
            pid_t pid;
            int p[2];
            pipe(p);
            bool no_vfork = registers[IM] == 1 || registers[IM] == 3 || registers[IM] == 5 || registers[IM] == 7;
            if (no_vfork)
            {
                pid = fork();
            }
            else
            {
                pid = vfork();
            }
            if (pid == 0) {
                if (no_vfork) {
                    printf("input a character: ");
                    int got = getchar();
                    char buf[32];
                    snprintf(buf, sizeof(buf) - 1, "%d", got);
                    write(p[1], buf, sizeof(buf) - 1);
                    _exit(0);
                } else {
                    printf("input a character: ");
                    registers[instruction[1]] = getchar();
                    _exit(0);
                }
            }
            if (no_vfork) {
                char data[32];
                read(p[0], data , sizeof(data) - 1);
                registers[instruction[1]] = atoi(data);
            }
            break;
        }
    }
    default:
    {
        registers[ER] = INVALID;
        break;
    }
    }
    check_err(instruction);
}

void begin_vm(Program *inprog)
{
    struct timespec freq = {0};
    struct timespec remaining;
    freq.tv_sec = 0;
    freq.tv_nsec = 1000;
    prog = *inprog;
    SP = -1;

    while (running && PC < prog.length)
    {
        eval(prog.data[PC]);
        if (!is_jmp)
        {
            PC++;
        }
        nanosleep(&freq, &remaining);
    }
}