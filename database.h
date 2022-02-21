#ifndef DBISLAB_DATABASE_H
#define DBISLAB_DATABASE_H

#include "albums.h"
#include "artists.h"

struct dataBase {
    int albumNumber;
    int artistNumber;
    int firstAlbumData;
    int firstArtistData;
    int fileSpaceArtists;
    int fileSpaceAlbums;
};

struct dataBase newDataBase();
char* processCommand(int argumentNumber, char* command[], struct dataBase* db);

char* init(struct dataBase* db);

char* getMHandler(int argc, char* command[], struct dataBase* db);
int getMaster(struct Artist* artist, int id, struct dataBase *db);

char* getSHandler(int argc, char* command[], struct dataBase* db);
int getSlave(struct Album* album, int id, int artist_id, struct dataBase* db);

char* delMHandler(int argc, char* command[], struct dataBase* db);
int delMaster(int id, struct dataBase* db);

char* delSHandler(int argc, char* command[], struct dataBase* db);
int delSlave(int id, int artist_id, struct dataBase* db);

char* updateMHandler(int argc, char* command[], struct dataBase* db);
int updateMaster(struct Artist artist, struct dataBase* db);

char* updateSHandler(int argc, char* command[], struct dataBase* db);
int updateSlave(struct Album album, struct dataBase* db);

char* insertMHandler(int argc, char* command[], struct dataBase* db);
int insertMaster(struct Artist artist, struct dataBase* db);

char* insertSHandler(int argc, char* command[], struct dataBase* db);
int insertSlave(struct Album album, struct dataBase* db);

const char* exitHandler();

#endif