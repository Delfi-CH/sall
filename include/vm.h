#define STACK_SIZE 256
#define MEM_SIZE 1024
#define MAX_PROGRAM_LENGTH 1024
#define LINE_LENGTH 5

#ifndef SALL_VM_H_

#define SALL_VM_H_
#include "sall.h"

extern int registers[REGISTER_SIZE];
extern int stack[STACK_SIZE];
extern int memory[MEM_SIZE];

typedef struct program {
    int data[MAX_PROGRAM_LENGTH][LINE_LENGTH];
    int line_lengths[MAX_PROGRAM_LENGTH];
    int length;
} Program;

void begin_vm(Program *prog);

void print_instruction(int instruction[LINE_LENGTH]);
void print_stack(int registers[REGISTER_SIZE], int stack[STACK_SIZE], int SP);
void print_registers(int registers[REGISTER_SIZE]);
void print_memory(int memory[MEM_SIZE]);

bool is_register(int value, int registers[REGISTER_SIZE]);

#endif