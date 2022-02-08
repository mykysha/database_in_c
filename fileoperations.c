#include <stdio.h>
#include <stdlib.h>
#include "fileoperations.h"

int writeToFile(char* contents, char* fileName) {
    FILE *file = NULL;

    int err = openFile(fileName, &file);

    if (err == 1) {
        return 1;
    }

    fprintf(file, contents);

    fflush(file);

    fclose(file);

    return 0;
}

int openFile(char *fileName, FILE **file) {
    *file = fopen(fileName, "w");

    if (*file == NULL) {
        printf("\nUnable to open file \'%s\'.\n", fileName);

        return 1;
    }

    return 0;
}
