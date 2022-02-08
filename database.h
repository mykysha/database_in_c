#ifndef DBISLAB_DATABASE_H
#define DBISLAB_DATABASE_H

#include "albums.h"
#include "artists.h"

#define maxDatabaseSize 100

struct dataBase {
    int albumNumber;
    struct Album albums[maxDatabaseSize];
    int artistNumber;
    struct Artist artists[maxDatabaseSize];
};

struct dataBase newDataBase();
char* processCommand(int,char* [], struct dataBase*);
char* getMFunc(int, char* [], struct dataBase*);
char* getSFunc(int, char* [], struct dataBase*);
char* delMFunc(int, char* [], struct dataBase*);
char* delSFunc(int, char* [], struct dataBase*);
char* updateMFunc(int, char* [], struct dataBase*);
char* updateSFunc(int, char* [], struct dataBase*);
char* insertMFunc(int, char* [], struct dataBase*);
char* insertSFunc(int, char* [], struct dataBase*);
char* countFunc(int, char* [], struct dataBase*);
char* exitFunc();

#endif