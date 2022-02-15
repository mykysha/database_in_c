#include <stdio.h>
#include <string.h>
#include "database.h"
#include "stringoperations.h"

const int maxCommandSymbolLength = 50;
const int maxCommandArgumentLength = 7;

int main() {
    char *res;
    char command[maxCommandSymbolLength];
    char *arguments[maxCommandArgumentLength];
    struct dataBase db = newDataBase();

    printf("please enter your command!\n");

    while (1) {
        printf("%s", "> ");

        fflush(stdout);

        scanf("%[^\n]%*c", command);

        int commandLen = findCommandLen(command);

        int argumentNumber = commandLen-1;

        char delim[] = " ";

        char *ptr = strtok(command, delim);
        int i = 0;

        while(ptr != NULL) {
            arguments[i] = ptr;

            i++;

            ptr = strtok(NULL, delim);
        }

        res = processCommand(argumentNumber, arguments, &db);

        if (strcmp(res, "exit") == 0) {
            break;
        }

        printf("\n%s\n", res);
    }

    return 0;
}