#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "database.h"
#include "fileoperations.h"
#include "stringoperations.h"

// Commands
const char getMCommand[] = "get-m";
const char getSCommand[] = "get-s";
const char delMCommand[] = "del-m";
const char delSCommand[] = "del-s";
const char updateMCommand[] = "update-m";
const char updateSCommand[] = "update-s";
const char insertMCommand[] = "insert-m";
const char insertSCommand[] = "insert-s";
const char countCommand[] = "count";
const char exitCommand[] = "exit";

// Errors
char errUnknownCommand[] = "Entered command is not defined.";
char errWrongArgumentNumber[] = "Entered command is valid, but number of arguments is wrong";
char errShouldBeNumber[] = "One of the values should be a number";
char errMasterFailure[] = "Failed to work with master file";
char errIndexFailure[] = "Failed to work with index file";
char errSlaveFailure[] = "Failed to work with slave file";
char errInit[] = "Failed to initialize database";
char errMasterNotExists[] = "Artist with such id does not exist in current database";
char errSlaveNotExists[] = "Slave with such id does not exist in current database";
char errNoSlaves[] = "This master has no slaves";
char errUnhandled[] = "Received critical error while doing this operation";

char* processCommand(int argumentNumber, char* command[], struct dataBase* db) {
    char *argument = command[0];

    command++;

    if (strcmp(argument, getMCommand) == 0) {
        return getMHandler(argumentNumber, command, db);

    } else if (strcmp(argument, getSCommand) == 0) {
        return getSHandler(argumentNumber, command, db);

    } else if (strcmp(argument, delMCommand) == 0) {
        return delMHandler(argumentNumber, command, db);

    } else if (strcmp(argument, delSCommand) == 0) {
        return delSHandler(argumentNumber, command, db);

    } else if (strcmp(argument, updateMCommand) == 0) {
        return updateMHandler(argumentNumber, command, db);

    } else if (strcmp(argument, updateSCommand) == 0) {
        return updateSHandler(argumentNumber, command, db);

    } else if (strcmp(argument, insertMCommand) == 0) {
        return insertMHandler(argumentNumber, command, db);

    } else if (strcmp(argument, insertSCommand) == 0) {
        return insertSHandler(argumentNumber, command, db);

    } else if (strcmp(argument, countCommand) == 0) {
        return countHandler(argumentNumber, db);

    } else if (strcmp(argument, exitCommand) == 0) {
        return exitHandler(argumentNumber);

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

char* getMHandler(int argc, char* command[], struct dataBase* db) {
    if (argc != 1) {
        return errWrongArgumentNumber;
    }

    if(!isNumber(command[0])) {
        return errShouldBeNumber;
    }

    int id = atoi(command[0]);

    struct Artist artist;

    int err = getMaster(&artist, id, db);
    if (err != 0) {
        if (err == 1) {
            return errMasterFailure;
        }

        return errUnhandled;
    }

    char res[maxStringLength];

    sprintf(res, "id: %d, name %s", artist.id, artist.name);

    printf("%s", res);

    return "read successfully";
}

int getMaster(struct Artist* artist, int id, struct dataBase* db) {
    struct Artist* readArtist = readMaster(id, db);

    if (readArtist == NULL) {
        return 1;
    }

    artist->id = readArtist->id;
    strcpy(artist->name, readArtist->name);

    return 0;
}

char* getSHandler(int argc, char* command[], struct dataBase* db) {
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

    int id = atoi(command[0]);

    struct Album album;

    int err = getSlave(&album, id, artist_id, db);
    if (err != 0) {
        if (err == -1) {
            return errNoSlaves;
        }

        if (err == 1) {
            return errSlaveNotExists;
        }

        if (err == 2) {
            return errSlaveFailure;
        }

        return errUnhandled;
    }

    char res[maxStringLength];

    sprintf(res, "id: %d, name: %s, artist id: %d, year: %d", album.id, album.name, album.artist_id, album.year);

    printf("%s", res);

    return "read successfully";
}

int getSlave(struct Album* album, int id, int artist_id, struct dataBase* db) {
    struct Artist* artist = readMaster(artist_id, db);
    if (artist == NULL) {
        return -1;
    }

    if (artist->slave1 == -1) {
        return -1;
    }

    int firstSlave = artist->slave1;

    int slaveAddress = getSlaveAddress(id, firstSlave);
    if (slaveAddress == -1) {
        return 1;
    }

    struct Album* readAlbum = getSlaveByAddress(slaveAddress);
    if (readAlbum == NULL) {
        return 2;
    }

    album->id = readAlbum->id;
    strcpy(album->name, readAlbum->name);
    album->artist_id = readAlbum->artist_id;
    album->year = readAlbum->year;

    return 0;
}

char* delMHandler(int argc, char* command[], struct dataBase* db) {
    if (argc != 1) {
        return errWrongArgumentNumber;
    }

    if(!isNumber(command[0])) {
        return errShouldBeNumber;
    }

    int id = atoi(command[0]);

    int err = delMaster(id, db);
    if (err != 0) {
        if (err == 1) {
            return errMasterNotExists;
        }

        if (err == 2) {
            return errSlaveFailure;
        }

        if (err == 3) {
            return errMasterFailure;
        }

        if (err == 4) {
            return errIndexFailure;
        }

        return errUnhandled;
    }

    return "deleted successfully";
}

int delMaster(int id, struct dataBase* db) {
    int address = readIndex(id, db);

    if(address == -1) {
        return 1;
    } else {
        struct Artist* artist = getMasterByAddress(address);

        int err = 0;

        err = deleteAllSlaves(artist->slave1);
        if (err != 0) {
            return 2;
        }

        err = deleteMaster(address);
        if (err != 0) {
            return 3;
        }

        err = deleteIndex(id, db);
        if (err != 0) {
            return 4;
        }
    }

    return 0;
}

char* delSHandler(int argc, char* command[], struct dataBase* db) {
    if (argc != 2) {
        return errWrongArgumentNumber;
    }

    if(!isNumber(command[0])) {
        return errShouldBeNumber;
    }

    if(!isNumber(command[1])) {
        return errShouldBeNumber;
    }

    int id = atoi(command[0]);

    int artist_id = atoi(command[1]);

    int err = delSlave(id, artist_id, db);
    if (err != 0) {
        if (err == 1) {
            return errNoSlaves;
        }

        if (err == 2) {
            return errSlaveNotExists;
        }

        return errUnhandled;
    }

    return "deleted successfully";
}

int delSlave(int id, int artist_id, struct dataBase* db) {
    struct Artist* artist = getMasterByAddress(artist_id);
    if (artist->slave1 == -1) {
        return 1;
    }

    int firstSlave = artist->slave1;


    int slaveAddress = getSlaveAddress(id, firstSlave);
    if (slaveAddress == -1) {
        return 2;
    }

    deleteSlave(slaveAddress);

    return 0;
}

char* updateMHandler(int argc, char* command[], struct dataBase* db) {
    if (argc != 2) {
        return errWrongArgumentNumber;
    }

    int id = atoi(command[0]);

    struct Artist artist;

    artist.id = id;
    strcpy(artist.name, command[1]);

    int oldAddr = readIndex(artist.id, db);

    struct Artist* oldArtist = getMasterByAddress(oldAddr);

    artist.slave1 = oldArtist->slave1;

    int err = updateMaster(artist, db);
    if (err != 0) {
        if (err == 1) {
            return errMasterFailure;
        }

        if (err == 2) {
            return errIndexFailure;
        }

        return errUnhandled;
    }

    printf("id: %d", artist.id);

    return "insert done successfully";
}

int updateMaster(struct Artist artist, struct dataBase* db) {
    int oldAddr = readIndex(artist.id, db);

    deleteMaster(oldAddr);
    deleteIndex(artist.id, db);

    db->fileSpaceArtists++;

    struct ArtistInd artistInd;

    int address = writeToMaster(artist);
    if (address == -1) {
        return 1;
    }

    artistInd.id = artist.id;
    artistInd.address = address;

    int err = writeToIndex(artistInd);
    if (err == 1) {
        return 2;
    }

    save(db);

    return 0;
}

char* updateSHandler(int argc, char* command[], struct dataBase* db) {
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

    album.id = id;
    strcpy(album.name, command[1]);
    album.year = year;
    album.artist_id = artist_id;

    int address = readIndex(album.artist_id, db);

    struct Artist* artist;

    artist = getMasterByAddress(address);

    int oldAddr = getSlaveAddress(album.id, artist->slave1);
    int albumNextSlave = getSlaveByAddress(oldAddr)->nextSlave;

    album.nextSlave = albumNextSlave;

    int err = updateSlave(album, db);
    if (err != 0) {
        if (err == 1) {
            return errMasterNotExists;
        }

        if (err == 2) {
            return errSlaveFailure;
        }

        if (err == 3) {
            return errMasterFailure;
        }

        return errUnhandled;
    }

    printf("id: %d", album.id);

    return "insert done successfully";
}

int updateSlave(struct Album album, struct dataBase* db) {
    int address = readIndex(album.artist_id, db);

    if (address == -1) {
        return 1;
    }

    struct Artist* artist;

    artist = getMasterByAddress(address);

    int oldAddr = getSlaveAddress(album.id, artist->slave1);

    int err = deleteSlave(oldAddr);
    if (err != 0) {
        return 2;
    }

    db->fileSpaceAlbums++;

    int slaveAddress = writeToSlave(album);

    save(db);

    artist = getMasterByAddress(address);

    int nextSlave;

    if (artist->slave1 == oldAddr) {
        artist->slave1 = slaveAddress;

        err = updateMaster(*artist, db);
        if (err != 0) {
            return 3;
        }
    } else {
        nextSlave = artist->slave1;

        for (;;) {
            struct Album* slaveAlbum = getSlaveByAddress(nextSlave);

            if (slaveAlbum->nextSlave == oldAddr) {
                slaveAlbum->nextSlave = slaveAddress;

                err = updateSlave(*slaveAlbum, db);
                if (err != 0) {
                    return 2;
                }

                break;
            }

            nextSlave = album.nextSlave;
        }
    }

    save(db);

    return 0;
}

char* insertMHandler(int argc, char* command[], struct dataBase* db) {
    if (argc != 1) {
        return errWrongArgumentNumber;
    }

    struct Artist artist;
    artist.id = db->artistNumber+1;
    strcpy(artist.name, command[0]);
    artist.slave1 = -1;

    int err = insertMaster(artist, db);
    if (err != 0) {
        if (err == 1) {
            return errMasterFailure;
        }

        if (err == 2) {
            return errIndexFailure;
        }

        return errUnhandled;
    }

    printf("id: %d", artist.id);

    return "insert done successfully";
}

int insertMaster(struct Artist artist, struct dataBase* db) {
    struct ArtistInd artistInd;

    db->artistNumber++;
    db->fileSpaceArtists++;

    int address = writeToMaster(artist);
    if (address == -1) {
        return 1;
    }

    artistInd.id = db->artistNumber;
    artistInd.address = address;

    int err = writeToIndex(artistInd);
    if (err == 1) {
        return 2;
    }

    save(db);

    return 0;
}

char* insertSHandler(int argc, char* command[], struct dataBase* db) {
    if (argc != 3) {
        return errWrongArgumentNumber;
    }

    if (!isNumber(command[1])) {
        return errShouldBeNumber;
    }

    int artist_id = atoi(command[1]);
    int year = atoi(command[2]);

    struct Album album;

    album.id = db->albumNumber+1;
    strcpy(album.name, command[0]);
    album.year = year;
    album.artist_id = artist_id;
    album.nextSlave = -1;

    int err = insertSlave(album, db);
    if (err != 0) {
        if (err == 1) {
            return errMasterNotExists;
        }

        if (err == 2) {
            return errSlaveFailure;
        }

        return errUnhandled;
    }

    printf("id: %d", album.id);

    return "insert done successfully";
}

int insertSlave(struct Album album, struct dataBase* db) {
    int address = readIndex(album.artist_id, db);

    if (address == -1) {
        return 1;
    }

    db->fileSpaceAlbums++;
    db->albumNumber++;

    int slaveAddress = writeToSlave(album);

    save(db);

    struct Artist* artist = getMasterByAddress(address);

    int nextSlave;

    if (artist->slave1 == -1) {
        artist->slave1 = slaveAddress;

        updateMaster(*artist, db);
    } else {
        nextSlave = artist->slave1;

        for (;;) {
            struct Album* slaveAlbum = getSlaveByAddress(nextSlave);

            if (slaveAlbum->nextSlave == -1) {
                slaveAlbum->nextSlave = slaveAddress;

                int err = updateSlave(*slaveAlbum, db);
                if (err != 0) {
                    return 2;
                }

                break;
            }

            nextSlave = album.nextSlave;
        }
    }

    save(db);

    return 0;
}

char* countHandler(int argc, struct dataBase* db) {
    if (argc != 0) {
        return errWrongArgumentNumber;
    }

    char res[maxStringLength];

    sprintf(res, "number of albums written to db: %d, number of artists written to db: %d", db->albumNumber, db->artistNumber);

    printf("%s", res);

    return "read successfully";
}

const char* exitHandler(int argc) {
    if (argc != 0) {
        return errWrongArgumentNumber;
    }

    return exitCommand;
}

struct dataBase newDataBase() {
    struct dataBase db;
    init(&db);

    return db;
}
