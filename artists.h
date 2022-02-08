#ifndef DBISLAB_ARTISTS_H
#define DBISLAB_ARTISTS_H

#define maxArtistLineLength 50

struct Artist {
    int id;
    char name[maxArtistLineLength];
};

#endif