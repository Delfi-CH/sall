#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../../include/sall.h"
#include "../../include/vm.h"

int main(int argc, char **argv)
{
    if (argc <= 1)
    {
        printf("error: no input files\n");
        return -1;
    }

    Program prog = {0};

    char *filename = argv[1];

    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("error: could not read file `%s`\n", filename);
        return -1;
    }

    char line[512];

    char *firstline = fgets(line, sizeof line, file);

    if (strcmp(firstline, file_header)) {
        printf("error: invalid program, missing or invalid file header!\n");
        printf("your program may be corrupted, try re-assembling it\n");
        return -1;
    }

    while (fgets(line, sizeof line, file) && prog.length < MAX_PROGRAM_LENGTH)
    {
        int arr[LINE_LENGTH];
        int index = 0;

        char *token = strtok(line, " \t\n");

        while (token != NULL && index < LINE_LENGTH)
        {
            prog.data[prog.length][index++] = atoi(token);
            token = strtok(NULL, " \t\n");
        }

        prog.line_lengths[prog.length] = index;
        prog.length++;
    }

    fclose(file);

    begin_vm(&prog);

    return 0;
}