#ifndef DBISLAB_FILEOPERATIONS_H
#define DBISLAB_FILEOPERATIONS_H

#include "database.h"

int writeToMaster(struct Artist artist);
int writeToIndex(struct ArtistInd artistInd);
int writeToSlave(struct Album album);

struct Artist* readMaster(int id, struct dataBase* db);
int readIndex(int id, struct dataBase* db);

struct Artist* getMasterByAddress(int address);
struct Album* getSlaveByAddress(int address);
int getSlaveAddress(int id, int fistSlave);

int deleteMaster(int address);
int deleteIndex(int id, struct dataBase* db);
int deleteAllSlaves(int masterAddress);
int deleteSlave(int address);

int openFile(const char* fileName, char* mode, FILE** file);
int save(struct dataBase* db);
int initialFileRead(struct dataBase* db);

#endif
