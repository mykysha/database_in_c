#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "stringoperations.h"

int findCommandLen(char command[]) {
    char *space = " ";
    int what_len = strlen(space);
    int count = 0;

    char *where = command;

    if (what_len) {
        while ((where = strstr(where, space))) {
            where += what_len;
            count++;
        }
    }

    count++;

    return count;
}

bool isNumber(const char* line) {
    for(int i = 0; line[i] != '\0'; ++i) {
        if (isdigit(line[i]) == 0) return false;
    }

    return true;
}