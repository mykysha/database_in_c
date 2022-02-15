#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include "artists.h"
#include "albums.h"
#include "fileoperations.h"
#include "stringoperations.h"
#include "database.h"

// Files
const char slaveFile[] = "albums.fl";
const char masterFile[] = "artists.fl";
const char indexFile[] = "artists.ind";

int writeToMaster(struct Artist artist) {
    FILE *file = NULL;

    int err = openFile(masterFile,"awb", &file);
    if (err == 1) {
        return -1;
    }

    fseek(file, 0, SEEK_END);

    int address = ftell(file);

    fwrite(&artist, sizeof(struct Artist), 1, file);

    fflush(file);

    fclose(file);

    return address;
}

int writeToIndex(struct ArtistInd artistInd) {
    FILE *file = NULL;

    int err = openFile(indexFile,"awb", &file);
    if (err == 1) {
        return 1;
    }

    fseek(file, 0, SEEK_END);

    fwrite(&artistInd, sizeof(struct ArtistInd), 1, file);

    fflush(file);

    fclose(file);

    return 0;
}

int writeToSlave(struct Album album) {
    FILE* file;

    int err = openFile(slaveFile, "r+b", &file);
    if (err == 1) {
        return -1;
    }

    int address = ftell(file);

    fseek(file, address, SEEK_SET);

    fwrite(&album, sizeof(struct Album), 1, file);

    fflush(file);

    fclose(file);

    return address;
}

struct Artist* getMaster(int id, struct dataBase* db) {
    FILE* mFIle;
    int err;

    struct Artist* artist;

    err = openFile(masterFile, "rb", &mFIle);
    if (err == 1) {
        return NULL;
    }

    int address = getIndex(id, db);

    if (address == -1) {
        return NULL;
    } else {
        artist = getMasterByAddress(address);
    }

    fclose(mFIle);

    return artist;
}

int getIndex(int id, struct dataBase* db) {
    FILE* file;
    int err;

    int address = -1;

    struct ArtistInd artistInd;

    err = openFile(indexFile, "rb", &file);
    if (err == 1) {
        return -1;
    }

    fseek(file, db->firstArtistData, SEEK_SET);

    for(int i = 0; i < db->fileSpaceArtists; i++) {
        fread(&artistInd, sizeof(struct ArtistInd), 1, file);

        if(artistInd.id == id) {
            address = artistInd.address;

            break;
        }
    }

    fclose(file);

    return address;
}

int getSlaveAddress(int id, int fistSlave) {
    FILE* file;

    int err = openFile(slaveFile, "r+b", &file);
    if (err == 1) {
        return -1;
    }

    struct Album* album;

    int address = fistSlave;

    for(;;) {
        album = getSlaveByAddress(address);

        if(album->id == id) {
            return address;
        }

        if (album->nextSlave == -1) {
            return -1;
        }

        address = album->nextSlave;
    }
}

struct Artist* getMasterByAddress(int address) {
    FILE* mFIle;
    int err;

    struct Artist artist;

    err = openFile(masterFile, "rb", &mFIle);
    if (err == 1) {
        return NULL;
    }

    fseek(mFIle, address, SEEK_SET);
    fread(&artist, sizeof(struct Artist), 1, mFIle);

    fclose(mFIle);

    return &artist;
}

struct Album* getSlaveByAddress(int address) {
    FILE* file;
    int err;

    struct Album album;

    err = openFile(slaveFile, "rb", &file);
    if (err == 1) {
        return NULL;
    }

    fseek(file, address, SEEK_SET);
    fread(&album, sizeof(struct Album), 1, file);

    fclose(file);

    return &album;
}

int deleteMaster(int address) {
    FILE* file;
    int err;

    struct Artist emptyArtist;
    emptyArtist.id = 0;
    emptyArtist.slave1 = 0;
    strcpy(emptyArtist.name, "");

    err = openFile(masterFile, "rb", &file);
    if (err == 1) {
        return NULL;
    }

    fseek(file, address, SEEK_SET);
    fwrite(&emptyArtist, sizeof(struct Artist), 1, file);

    fflush(file);

    fclose(file);

    return 0;
}

int deleteIndex(int id, struct dataBase* db) {
    FILE* file;
    int err;

    int address = -1;

    struct ArtistInd artistInd;

    fseek(file, db->firstArtistData, SEEK_SET);
    for(int i = 0; i < db->fileSpaceArtists; ++i) {
        fread(&artistInd, sizeof(struct ArtistInd), 1, file);

        if(artistInd.id == id) {
            address = ftell(file) - sizeof(struct ArtistInd);

            break;
        }
    }

    if(address == -1) {
        return 0;
    } else {
        fseek(file, address, SEEK_SET);

        struct ArtistInd emptyInd;
        emptyInd.id = 0;
        emptyInd.address = 0;

        fwrite(&emptyInd, sizeof(struct ArtistInd), 1, file);
    }

    fclose(file);

    return 0;
}

int deleteAllSlaves(int masterAddress) {
    int address = masterAddress;
    if (address == -1) {
        return 0;
    }

    for (;;) {
        struct Album* album = getSlaveByAddress(address);

        deleteSlave(address);

        address = album->nextSlave;
        if (address == -1) {
            break;
        }
    }

    return 0;
}

int deleteSlave(int address) {
    FILE* file;
    int err;

    struct Album emptyAlbum;
    emptyAlbum.id = 0;
    strcpy(emptyAlbum.name, "");
    emptyAlbum.nextSlave = 0;
    emptyAlbum.artist_id = 0;
    emptyAlbum.year = 0;

    err = openFile(slaveFile, "rb", &file);
    if (err == 1) {
        return 1;
    }

    fseek(file, address, SEEK_SET);
    fwrite(&emptyAlbum, sizeof(struct Album), 1, file);

    fflush(file);

    fclose(file);

    return 0;
}

int initialFileRead(struct dataBase* db) {
    FILE *indFile, *mFile;
    int err;

    err = openFile(indexFile, "a+b", &indFile);
    if(err == 1) {
        return 1;
    }

    if(fseek(indFile, 0, SEEK_END) == 0 && ftell(indFile) == 0) {
        db->firstArtistData = sizeof(*db);
        db->fileSpaceArtists = 0;
        db->artistNumber = 0;

        fwrite(db, sizeof(*db), 1, indFile);

        err = openFile(masterFile, "wb", &mFile);
        if(err == 1) {
            return 1;
        }

        fclose(mFile);
    } else {
        fseek(indFile, 0, SEEK_SET);
        fread(db, sizeof(*db), 1, indFile);
    }

    fclose(indFile);

    FILE *slFile;

    err = openFile(slaveFile, "a+b", &slFile);
    if(err == 1) {
        return 1;
    }

    if(fseek(slFile, 0, SEEK_END) == 0 && ftell(slFile) == 0) {
        db->firstAlbumData = sizeof(*db);
        db->fileSpaceAlbums = 0;
        db->albumNumber = 0;

        fwrite(db, sizeof(*db), 1, slFile);
    } else {
        fseek(slFile, 0, SEEK_SET);
        fread(db, sizeof(*db), 1, slFile);
    }

    fclose(slFile);

    return 0;
}

int save(struct dataBase* db) {
    FILE* fileInd;

    int err = openFile(indexFile, "r+b", &fileInd);
    if (err == 1) {
        return 1;
    }

    FILE* fileSlave;

    err = openFile(slaveFile, "r+b", &fileSlave);
    if (err == 1) {
        return 1;
    }

    rewind(fileInd);
    rewind(fileSlave);

    fwrite(db, sizeof(struct dataBase), 1, fileInd);
    fwrite(db, sizeof(struct dataBase), 1, fileSlave);

    fclose(fileInd);
    fclose(fileSlave);

    return 0;
}

int openFile(const char *fileName, char* mode, FILE **file) {
    *file = fopen(fileName, mode);

    if (*file == NULL) {
        printf("\nUnable to open file \'%s\'.\n", fileName);

        return 1;
    }

    return 0;
}
