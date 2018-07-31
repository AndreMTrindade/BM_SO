#ifndef CLIENTMANAGEMENT_H
#define CLIENTMANAGEMENT_H

#include "../EstruturasComunicacao.h"

#ifdef __cplusplus
extern "C" {
#endif

///ADD A CLIENT TO LIST
Client* AddClient(Word *p, Client *lClients);

///KICK PLAYER
void KickPlayer(Word *p, Client *lClients);

///SHOW ALL USERS REGISTERED
void Users(Client *lClients);

////CHECK STATUS OF CLIENT
int CheckClient(Client *cr, Client *lClients);

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

