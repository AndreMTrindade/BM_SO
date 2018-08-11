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
#include "EstruturasComunicacao.h"

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


Client* AddClient(Word *p, Client *lClients) {

    Word *it;
    Word *Username, *password;
    pthread_t recebe;

    int i = 0;

    it = p;

    while (it != NULL) {

        if (i == 1) {
            Username = it;
        }
        if (i == 2) {
            password = it;
        }
        i++;
        it = (Word*) it->p;
    }

    if (lClients == NULL) {

        lClients = (Client*) malloc(sizeof (Client));

        strcpy(lClients->name, Username->command);
        strcpy(lClients->passWord, password->command);
        lClients->PID = 0;
        lClients->status = 0;
        lClients->p = NULL;
        printf("Primeiro Utilizador Adicionado com Sucesso!\n");

        return lClients;
    } else {
        Client *it;
        Client *novo;
        it = lClients;
        while (it->p != NULL) {
            it = (Client*) it->p;
        }
        novo = (Client*) malloc(sizeof (Client));
        strcpy(novo->name, Username->command);
        strcpy(novo->passWord, password->command);
        novo->PID = 0;
        novo->status = 0;
        novo->p = NULL;
        it->p = novo;
        printf("Utilizador Adicionado com Sucesso!\n");
    }
    return lClients;
}

void Users(Client *lClients) {
    Client *it;
    int i = 1;
    printf("Utilizadores: \n\n");
    it = lClients;

    while (it != NULL) {
        printf("%d - %d - %s\n", i, it->status, it->name);
        fflush(stdout);
        it = (Client*) it->p;
        i++;
    }
}

int Count(Client *lClients) {
    int i = 0;

    lClients = lClients->p;

    while (lClients != NULL) {
        if (lClients->status == 1) {
            i++;
        }
        lClients = lClients->p;
    }

    return i;
}

void KickPlayer(Word *p, Client *lClients) {
    Client *it;

    it = lClients;
    char *name;
    Word *pa = (Word*) p->p;
    name = pa->command;
    while (it != NULL) {
        if (strcmp(it->name, name) == 0) {
            it->status = -1;
            printf("Utilizador %s fora do Jogo!\n", it->name);
            return;
        }
        it = it->p;
    }
}

int CheckClient(Client cr, Client *lClients) {
    Client *it, *temp;
    it = lClients;
    if (cr.status == 3) {
        while (it->p != NULL) {
            it = it->p;
        }

        temp = (Client*) malloc(sizeof (Client));
        temp->PID = cr.PID;
        temp->status = 3;
        temp->p = NULL;
        it->p = temp;
        return 1;
    }

    it = lClients;
    while (it != NULL) {
        if (strcmp(cr.name, it->name) == 0 && strcmp(cr.passWord, it->passWord) == 0) {
            if (it->status == 0) {
                it->status = 1;
                it->PID = cr.PID;
                return 1;
            } else {
                return 0;
            }
        }
        it = (Client*) it->p;
    }
    return -1;
}

Client* ReadClients(){
    
    FILE *fd = fopen("clients.txt", "rt");
    Client *clients = malloc(sizeof (Client));
    strcpy(clients->name, "Admin");
    strcpy(clients->passWord, "Admin");
    clients->PID = -1;
    clients->p = NULL;

    if (fd == NULL) {
        printf("<AVISO> Ainda Nao tem Clientes\n");
        fflush(stdout);
        return NULL;
    }

    char name[50];
    char pass[50];
    Client *it;
    Client *new;



    it = clients;

    while (fscanf(fd, "%s %s", name, pass) > 0) {
        new = (Client*) malloc(sizeof (Client));
        strcpy(new->name, name);
        strcpy(new->passWord, pass);
        new->PID = 0;
        new->status = 0;
        new->p = NULL;
        it->p = new;
        it = new;
    }

    it->p = NULL;
    fclose(fd);
    return clients;
}

void SaveClients(Client *lClients) {
    unlink("clients.txt");
    FILE *fd = fopen("clients.txt", "wt");
    Client *it;

    it = lClients;
    it = it->p;
    while (it != NULL) {
        fprintf(fd, "%s %s\n", it->name, it->passWord);
        it = (Client*) it->p;
    }
    fclose(fd);
    return;
}

#ifdef __cplusplus
}
#endif

#endif /* CLIENTMANAGEMENT_H */

