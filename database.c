#include <string.h>
#include <stdio.h>
#include <stdlib.h>
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
const char exitCmd[] = "exit";

// Errors
char errUnknownCommand[] = "Entered command is not defined.";
char errWrongArgumentNumber[] = "Entered command is valid, but number of arguments is wrong";
char errShouldBeNumber[] = "One of the values should be a number";
char errMasterFailure[] = "Failed to work with master file";
char errIndexFailure[] = "Failed to work with index file";
char errSlaveFailure[] = "Failed to work with slave file";
char errInit[] = "Failed to initialize database";
char errArtistNotExists[] = "Artist with such id does not exist in current database";
char errSlaveNotExists[] = "Slave with such id does not exist in current database";
char errNoSlaves[] = "This master has no slaves";

// Other
char nothingToReturn[] = "Nothing to return yet";

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
    } else if (strcmp(argument, exitCmd) == 0) {
        return exitFunc();
    } else {
        return errUnknownCommand;
    }
}

char* init(struct dataBase* db) {
    int err = initialFileRead(db);
    if (err == 1) {
        return errInit;
    }

    return "Database initialized";
}

char* getMFunc(int argc, char* command[], struct dataBase* db) {
    if (argc != 1) {
        return errWrongArgumentNumber;
    }

    if(!isNumber(command[0])) {
        return errShouldBeNumber;
    }

    int id = atoi(command[0]);

    struct Artist artist = &getMaster(id, db);

    if (artist == NULL) {
        return errMasterFailure;
    }

    char res[maxStringLength];

    sprintf(res, "id: %d, name %s", artist.id, artist.name);

    printf("%s", res);

    return "read successfully";
}

char* getSFunc(int argc, char* command[], struct dataBase* db) {
    if (argc != 2) {
        return errWrongArgumentNumber;
    }

    if(!isNumber(command[0])) {
        return errShouldBeNumber;
    }

    if(!isNumber(command[1])) {
        return errShouldBeNumber;
    }

    int artist_id = atoi(command[1]);

    struct Artist* artist = getMasterByAddress(artist_id);
    if (artist->slave1 == -1) {
        return errNoSlaves;
    }

    int firstSlave = artist->slave1;

    int id = atoi(command[0]);

    int slaveAddress = getSlaveAddress(id, firstSlave);
    if (slaveAddress == -1) {
        return errSlaveNotExists;
    }

    struct Album *album = getSlaveByAddress(slaveAddress);
    if (album == NULL) {
        return errSlaveFailure;
    }

    return "read successfully";
}

char* delMFunc(int argc, char* command[], struct dataBase* db) {
    if (argc != 1) {
        return errWrongArgumentNumber;
    }

    if(!isNumber(command[0])) {
        return errShouldBeNumber;
    }

    int id = atoi(command[0]);

    int address = getIndex(id, db);

    if(address == -1) {
        return errArtistNotExists;
    } else {
        deleteAllSlaves(address);
        deleteMaster(address);
        deleteIndex(id, db);
    }

    return "deleted successfully";
}

char* delSFunc(int argc, char* command[], struct dataBase* db) {
    if (argc != 2) {
        return errWrongArgumentNumber;
    }

    if(!isNumber(command[0])) {
        return errShouldBeNumber;
    }

    if(!isNumber(command[1])) {
        return errShouldBeNumber;
    }

    int artist_id = atoi(command[1]);

    struct Artist* artist = getMasterByAddress(artist_id);
    if (artist->slave1 == -1) {
        return errNoSlaves;
    }

    int firstSlave = artist->slave1;

    int id = atoi(command[0]);

    int slaveAddress = getSlaveAddress(id, firstSlave);
    if (slaveAddress == -1) {
        return errSlaveNotExists;
    }

    deleteSlave(slaveAddress);

    return nothingToReturn;
}

char* updateMFunc(int argc, char* command[], struct dataBase* db) {
    if (argc != 1) {
        return errWrongArgumentNumber;
    }

    int id = atoi(command[0]);

    int oldAddr = getIndex(id, db);
    deleteMaster(oldAddr);
    deleteIndex(id, db);

    struct Artist artist;
    struct ArtistInd artistInd;

    db->fileSpaceArtists++;

    artist.id = id;
    strcpy(artist.name, command[0]);
    artist.slave1 = -1;

    int address = writeToMaster(artist);
    if (address == -1) {
        return errMasterFailure;
    }

    artistInd.id = id;
    artistInd.address = address;

    int err = writeToIndex(artistInd);
    if (err == 1) {
        return errIndexFailure;
    }

    save(db);

    printf("id: %d", artist.id);

    return "insert done successfully";
}

char* updateSFunc(int argc, char* command[], struct dataBase* db) {
    if (argc != 4) {
        return errWrongArgumentNumber;
    }

    if (!isNumber(command[0])) {
        return errShouldBeNumber;
    }

    if (!isNumber(command[2])) {
        return errShouldBeNumber;
    }

    if (!isNumber(command[3])) {
        return errShouldBeNumber;
    }

    int id = atoi(command[0]);
    int artist_id = atoi(command[2]);
    int year = atoi(command[3]);

    struct Album album;

    int address = getIndex(artist_id, db);

    if (address == -1) {
        return errArtistNotExists;
    }

    struct Artist* artist;

    artist = getMasterByAddress(address);

    int oldAddr = getSlaveAddress(id, artist->slave1);

    deleteSlave(oldAddr);

    db->fileSpaceAlbums++;
    db->albumNumber++;

    album.id = id;
    strcpy(album.name, command[1]);
    album.year = year;
    album.artist_id = artist_id;
    album.nextSlave = -1;

    int slaveAddress = writeToSlave(album);

    artist = getMasterByAddress(address);

    int nextSlave;

    if (artist->slave1 == -1) {
        artist->slave1 = slaveAddress;
        updateMFunc(1, &artist->name, db);
    } else {
        nextSlave = artist->slave1;

        for (;;) {
            struct Album* slaveAlbum = getSlaveByAddress(nextSlave);

            if (slaveAlbum->nextSlave == -1) {
                slaveAlbum->nextSlave = slaveAddress;
                char cmd[3][maxStringLength] = {
                        (char) slaveAlbum->name,
                        slaveAlbum->artist_id,
                        slaveAlbum->year,
                };
                updateSFunc(3, cmd, db);
                break;
            }

            nextSlave = album.nextSlave;
        }
    }

    save(db);

    printf("id: %d", album.id);

    return "insert done successfully";

    return nothingToReturn;
}

char* insertMFunc(int argc, char* command[], struct dataBase* db) {
    if (argc != 1) {
        return errWrongArgumentNumber;
    }

    struct Artist artist;
    struct ArtistInd artistInd;

    db->artistNumber++;
    db->fileSpaceArtists++;

    artist.id = db->artistNumber;
    strcpy(artist.name, command[0]);
    artist.slave1 = -1;

    int address = writeToMaster(artist);
    if (address == -1) {
        return errMasterFailure;
    }

    artistInd.id = db->artistNumber;
    artistInd.address = address;

    int err = writeToIndex(artistInd);
    if (err == 1) {
        return errIndexFailure;
    }

    save(db);

    printf("id: %d", artist.id);

    return "insert done successfully";
}

char* insertSFunc(int argc, char* command[], struct dataBase* db) {
    if (argc != 3) {
        return errWrongArgumentNumber;
    }

    if (!isNumber(command[1])) {
        return errShouldBeNumber;
    }

    int artist_id = atoi(command[1]);
    int year = atoi(command[2]);

    struct Album album;

    int address = getIndex(artist_id, db);

    if (address == -1) {
        return errArtistNotExists;
    }

    db->fileSpaceAlbums++;
    db->albumNumber++;

    album.id = db->albumNumber;
    strcpy(album.name, command[0]);
    album.year = year;
    album.artist_id = artist_id;
    album.nextSlave = -1;

    int slaveAddress = writeToSlave(album);

    struct Artist* artist = getMasterByAddress(address);

    int nextSlave;

    if (artist->slave1 == -1) {
        artist->slave1 = slaveAddress;
        //todo update
    } else {
        nextSlave = artist->slave1;

        for (;;) {
            struct Album* slaveAlbum = getSlaveByAddress(nextSlave);

            if (slaveAlbum->nextSlave == -1) {
                slaveAlbum->nextSlave = slaveAddress;
                //todo update
                break;
            }

            nextSlave = album.nextSlave;
        }
    }

    save(db);

    printf("id: %d", album.id);

    return "insert done successfully";
}

const char* exitFunc() {
    return exitCmd;
}

struct dataBase newDataBase() {
    struct dataBase db;
    init(&db);

    return db;
}
