#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/tinyvm.h"

#define MAX_FILE_LENGTH 1024
#define LINE_LENGTH 512

typedef struct {
    int *arr;
    int length;
} LineResult;

LineResult *eval_line(char *line)
{
    char tokarr[5][64];
    char *token = strtok(line, " ");

    int count = 0;

    while (token != NULL)
    {
        strcpy(tokarr[count++], token);
        token = strtok(NULL, " ");
    }

    int *arr = malloc(count * sizeof(int));
    for (int i = 0; i < count; i++)
    {
        int got = atoi(tokarr[i]);
        if (got == 0)
        {
            int ins = str2ins(tokarr[i]);
            int reg = str2reg(tokarr[i]);
            if (ins >= 0)
            {
                got = ins;
            }
            else if (reg >= 0)
            {
                got = reg;
            }
        }
        arr[i] = got;
    }

    static LineResult result;
    result.arr = arr;
    result.length = count;

    return &result;
}

int main(int argc, char **argv)
{
    if (argc <= 2)
    {
        printf("error: no files set\n");
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file)
    {
        printf("error: could not read input file `%s`\n", argv[1]);
        return 1;
    }

    FILE *outfile = fopen(argv[2], "w");
    if (!outfile)
    {
        printf("error: could not write to outfile `%s`", argv[2]);
        return 1;
    }

    char *linearr[MAX_FILE_LENGTH];
    char line[LINE_LENGTH];
    int count = 0;

    while (count < MAX_FILE_LENGTH && fgets(line, sizeof line, file))
    {
        line[strcspn(line, "\n")] = '\0';

        linearr[count] = malloc(strlen(line) + 1);
        if (linearr[count] == NULL)
        {
            perror("malloc");
            break;
        }

        strcpy(linearr[count], line);
        count++;
    }

    fclose(file);

    for (int i = 0; i < count; i++)
    {
        char out[12];
        LineResult *result = eval_line(linearr[i]);
        for (int j = 0; j < result->length; j++)
        {
            snprintf(out, sizeof(out), "%d ", result->arr[j]);
            fputs(out, outfile);
        }
        fputs("\n", outfile);
        free(linearr[i]);
    }

    fclose(outfile);

    return 0;
}