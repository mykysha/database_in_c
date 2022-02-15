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
char* processCommand(int,char* [], struct dataBase*);
char* init(struct dataBase* db);
char* getMFunc(int, char* [], struct dataBase*);
char* getSFunc(int, char* [], struct dataBase*);
char* delMFunc(int, char* [], struct dataBase*);
char* delSFunc(int, char* [], struct dataBase*);
char* updateMFunc(int, char* [], struct dataBase*);
char* updateSFunc(int, char* [], struct dataBase*);
char* insertMFunc(int, char* [], struct dataBase*);
char* insertSFunc(int, char* [], struct dataBase*);
const char* exitFunc();

#endif