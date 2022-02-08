#ifndef DBISLAB_ALBUMS_H
#define DBISLAB_ALBUMS_H

# define maxAlbumLineLength 50

struct Album {
    int id;
    char name[maxAlbumLineLength];
    char artist[maxAlbumLineLength];
    int year;
};

#endif