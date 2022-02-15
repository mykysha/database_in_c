#ifndef DBISLAB_ARTISTS_H
#define DBISLAB_ARTISTS_H

#include <stdbool.h>

#define maxArtistLineLength 50

struct Artist {
    int id;
    char name[maxArtistLineLength];
    int slave1;
};

struct ArtistInd {
    int id;
    int address;
};

#endif