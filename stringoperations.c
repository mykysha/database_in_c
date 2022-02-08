#include <string.h>
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