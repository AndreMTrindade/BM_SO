#ifndef CLIENTMANAGEMENT_H
#define CLIENTMANAGEMENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include "../EstruturasComunicacao.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Word {
    char command[50];
    struct Word *p;
} Word;

///ADD A CLIENT TO LIST
Client* AddClient(Word *p, Client *lClients);

///KICK PLAYER
void KickPlayer(Word *p, Client *lClients);

///SHOW ALL USERS REGISTERED
void Users(Client *lClients);

////CHECK STATUS OF CLIENT
int CheckClient(Client cr, Client *lClients);

///RETURNS NUMBER OF PLAYERS TO PLAY
int Count(Client *lClients);

///READ CLIENTS FROM A FILE
Client* ReadClients();

///SAVE CLIENTS TO A FILE
void SaveClients(Client *lClients);

#ifdef __cplusplus
}
#endif

#endif /* CLIENTMANAGEMENT_H */

