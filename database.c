#include <string.h>
#include <stdio.h>
#include "database.h"
#include "fileoperations.h"
#include "stringoperations.h"

// Commands
const char getM[] = "get-m";
const char getS[] = "get-s";
const char delM[] = "del-m";
const char delS[] = "del-s";
const char updateM[] = "update-m";
const char updateS[] = "update-s";
const char insertM[] = "insert-m";
const char insertS[] = "insert-s";
const char count[] = "count";
const char exitCmd[] = "exit";

// Errors
char errUnknownCommand[] = "Entered command is not defined.";
char errWrongArgumentNumber[] = "Entered command is valid, but number of arguments is wrong";

// Other
char nothingToReturn[] = "Nothing to return yet";

// Files
const char masterFile[] = "albums.fl";
const char slaveFile[] = "artists.fl";
const char slaveFileIndex[] = "artists.ind";

char* processCommand(int argumentNumber, char* command[], struct dataBase* db) {
    char *argument = command[0];

    command++;

    if (strcmp(argument, getM) == 0) {
        return getMFunc(argumentNumber, command, db);
    } else if (strcmp(argument, getS) == 0) {
        return getSFunc(argumentNumber, command, db);
    } else if (strcmp(argument, delM) == 0) {
        return delMFunc(argumentNumber, command, db);
    } else if (strcmp(argument, delS) == 0) {
        return delSFunc(argumentNumber, command, db);
    } else if (strcmp(argument, updateM) == 0) {
        return updateMFunc(argumentNumber, command, db);
    } else if (strcmp(argument, updateS) == 0) {
        return updateSFunc(argumentNumber, command, db);
    } else if (strcmp(argument, insertM) == 0) {
        return insertMFunc(argumentNumber, command, db);
    } else if (strcmp(argument, insertS) == 0) {
        return insertSFunc(argumentNumber, command, db);
    } else if (strcmp(argument, count) == 0) {
        return countFunc(argumentNumber, command, db);
    } else if (strcmp(argument, exitCmd) == 0) {
        return exitFunc();
    } else {
        return errUnknownCommand;
    }
}

char* getMFunc(int argc, char* command[], struct dataBase* db) {
    if (argc != 1) {
        return errWrongArgumentNumber;
    }

    return nothingToReturn;
}

char* getSFunc(int argc, char* command[], struct dataBase* db) {
    if (argc != 2) {
        return errWrongArgumentNumber;
    }

    return nothingToReturn;
}

char* delMFunc(int argc, char* command[], struct dataBase* db) {
    if (argc != 1) {
        return errWrongArgumentNumber;
    }

    return nothingToReturn;
}

char* delSFunc(int argc, char* command[], struct dataBase* db) {
    if (argc != 2) {
        return errWrongArgumentNumber;
    }

    return nothingToReturn;
}

char* updateMFunc(int argc, char* command[], struct dataBase* db) {
    if (argc != 3) {
        return errWrongArgumentNumber;
    }

    return nothingToReturn;
}

char* updateSFunc(int argc, char* command[], struct dataBase* db) {
    if (argc != 4) {
        return errWrongArgumentNumber;
    }

    return nothingToReturn;
}

char* insertMFunc(int argc, char* command[], struct dataBase* db) {
    if (argc != 3) {
        return errWrongArgumentNumber;
    }

    return nothingToReturn;
}

char* insertSFunc(int argc, char* command[], struct dataBase* db) {
    if (argc != 1) {
        return errWrongArgumentNumber;
    }

    struct Artist newArtist;

    strcpy(newArtist.name, command[0]);

    db->artistNumber++;

    newArtist.id = db->artistNumber;

    db->artists[db->artistNumber-1] = newArtist;

    char line[maxStringLength];

    sprintf(line, "%d %s\n", newArtist.id, newArtist.name);

    char res[maxStringLength];

    int err = writeToFile(line, slaveFile);
    if (err == 1) {
        sprintf(res, "failed to store artist %s", newArtist.name);

        return res;
    }

    return "insert done successfully";
}

char* countFunc(int argc, char* command[], struct dataBase* db) {
    if (argc != 2) {
        return errWrongArgumentNumber;
    }

    return nothingToReturn;
}

char* exitFunc() {
    return exitCmd;
}

struct dataBase newDataBase() {
    struct dataBase db;
    db.artistNumber = 0;
    db.albumNumber = 0;

    return db;
}
