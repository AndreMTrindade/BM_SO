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

#include "ClientManagement.h"
#include "SOTools.h"

static int id = 0;

typedef struct {
    Client *lClients;
    Object *lObjects;
    int* exit;
} ThreadReferences;

typedef struct {
    Client *lClients;
    Object *lObjects;
    Object *enemy;
    int* exit;
} ThreadReferencesEnemy;


pthread_mutex_t lockFifoJogo;
pthread_mutex_t modifyList;

Client* Console(Client *clientes);
int IdentifyCommand(Word *p);
Object* ReadMaze();
void *ClientLoginRequest(void *dados);
Object CreateInitialObjects(Object *lObjects, Client *lClients);
void PutPlayer(Object *new, Object *lObjects);
void *Game(void *dados);
void SendAll(Object new, Client *lClients);
void ActionPlayer(Object *lObjects, Play play, Client *lClients);
int CheckMovement(Object *lObjects, Object *object, Play play);
void PutEnemys(Object *new, Object *lObjects);
void *MoveEnemy(void *dados);

int main(int argc, char** argv) {
    Client *lClients = ReadClients();
    if (access(FIFO_LOGIN, F_OK) == 0) {
        printf("Já está a ser executado um Servidor!..\n");
        return 1;
    }

    lClients = Console(lClients);
    SaveClients(lClients);

    return (EXIT_SUCCESS);
}

Client* Console(Client *clientes) {
    char comando[50];
    Word *it;
    Word *p;

    int op;
    int Exit = 0;
    pthread_t recebe;
    Object *lObjects = NULL;

    ThreadReferences *x = (ThreadReferences*) malloc(sizeof (ThreadReferences));

    lObjects = ReadMaze();

    x->lClients = clientes;
    x->exit = &Exit;
    x->lObjects = lObjects;

    pthread_create(&recebe, NULL, &ClientLoginRequest, (void *) x);

    while (1) {
        printf("Comando: ");
        scanf("%[^\n]", comando);
        CleanStdin();

        p = getWords(comando);
        op = IdentifyCommand(p);

        switch (op) {
            case -2:
                break;
            case -1:
                printf("<ERRO> Comando incorreto\n");
                break;
            case 0:
                clientes = AddClient(p, clientes);
                break;
            case 1:
                Users(clientes);
                break;
            case 2:
                KickPlayer(p, clientes);
                break;
            case 3:
                break;
            case 4:
                //  Shutdown(clientes);
                break;
            case 5:
                break;

        }

        it = p;
        while (p != NULL) {
            it = (Word*) it->p;
            free(p);
            p = it;
        }
        if (op == -2) {
            Exit = 1;
            unlink(FIFO_LOGIN);
            //pthread_mutex_destroy(&bloqueiaBomba);
            return clientes;
        }
    }
}

Object* ReadMaze() {
    FILE *fd = fopen("../Maze.txt", "rt");
    char c;

    if (fd == NULL) {
        return NULL;
    }

    Object *lObjects = NULL;
    Object *last = NULL;
    int x = 1, y = 0;

    while ((c = getc(fd)) != EOF) {
        if (c == '\n') {
            x = 0;
            y++;
        } else {
            if (c == '1') {
                if (lObjects == NULL) {
                    lObjects = (Object*) malloc(sizeof (Object));
                    lObjects->status = 1;
                    lObjects->id = ++id;
                    lObjects->type = 0;
                    lObjects->x = x;
                    lObjects->y = y;
                    lObjects->p = NULL;
                    last = lObjects;
                } else {
                    last->p = (Object*) malloc(sizeof (Object));
                    last->p->status = 1;
                    last->p->id = ++id;
                    last->p->x = x;
                    last->p->y = y;
                    last->p->type = 0;
                    last->p->p = NULL;
                    last = last->p;
                }
            }
        }
        x++;
    }
    return lObjects;
}

int IdentifyCommand(Word *p) {
    if (p == NULL) {
        printf("aa");
        return -1;
    } else {
        if (strcmp(UpString(p->command), "EXIT") == 0) {
            return -2;
        } else {
            if (strcmp(UpString(p->command), "ADD") == 0) {
                if (Size(p) == 3) {
                    return 0;
                }
            } else {
                if (strcmp(UpString(p->command), "USERS") == 0) {
                    return 1;

                } else {
                    if (strcmp(UpString(p->command), "KICK") == 0) {
                        if (Size(p) == 2) {
                            return 2;
                        }
                    } else {
                        if (strcmp(UpString(p->command), "GAME") == 0) {
                            if (Size(p) == 1) {
                                return 3;
                            }
                        } else {
                            if (strcmp(UpString(p->command), "SHUTDOWN") == 0) {
                                if (Size(p) == 1) {
                                    return 4;
                                }
                            } else {
                                if (strcmp(UpString(p->command), "MAP") == 0) {
                                    if (Size(p) == 2) {
                                        return 5;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return -1;
}

void *ClientLoginRequest(void *dados) {
    char str[80];
    int fd, fd_resp, i;
    int res;
    int *Exit;

    Client newRequest;

    Object *lObjects;
    ThreadReferences *x = (ThreadReferences*) dados;
    pthread_t envia;

    const char* s = getenv("NMAXPLAY");
    int nPlayers = 0;

    if (s == NULL) {
        srand((unsigned) time(NULL));
        nPlayers = 1 + (rand() % 20);
    } else {
        nPlayers = atoi(s);
    }

    //    printf("Players: %d\n", nPlayers);
    //    fflush(stdout);

    Client *lClients = x->lClients;
    lObjects = x->lObjects;

    Exit = x->exit;

    mkfifo(FIFO_LOGIN, 0600);

    fd = open(FIFO_LOGIN, O_RDWR);

    while (*Exit == 0) {
        i = read(fd, &newRequest, sizeof (newRequest));

        if (i == sizeof (newRequest)) {
            sprintf(str, "../JJJ%d", newRequest.PID);

            fd_resp = open(str, O_WRONLY);
            if (fd_resp == -1) {
                printf("Erro %d\n", newRequest.PID);
                fflush(stdout);
            } else {
                res = CheckClient(newRequest, lClients);
                write(fd_resp, &res, sizeof (res));
                close(fd_resp);
                if (Count(lClients) == 1) {//ALTERAR
                    printf("\nLimite de Clientes atingido\n");
                    fflush(stdout);
                    close(fd);
                    unlink(FIFO_LOGIN);
                    sleep(1);
                    pthread_create(&envia, NULL, &Game, (void *) x);
                    pthread_exit(0);
                }
            }
        }

    }

    close(fd);
    unlink(FIFO_LOGIN);
    pthread_exit(0);

}

Object CreateInitialObjects(Object *lObjects, Client *lClients) {

    Client *it;
    Object *itb;
    Object *ult;
    Object *new;
    PlayerInfo *info;

    int nMaxPantano;
    int nMaxEnemy, nMaxObjects;
    int i;
    const char* en = getenv("NENEMY");
    const char* ob = getenv("NOBJECT");

    it = lClients;
    itb = lObjects;

    //    if (en == NULL) {
    //        srand((unsigned) time(NULL));
    //        nMaxEnemy = 1 + (rand() % 10);
    //    } else {
    //        nMaxEnemy = atoi(en);
    //    }
    //    
    //     if (ob == NULL) {
    //        srand((unsigned) time(NULL));
    //        nMaxObjects = 1 + (rand() % 10);
    //    } else {
    //        nMaxObjects = atoi(ob);
    //    }

    while (itb->p != NULL) {
        itb = itb->p;
    }
    ult = itb;

    while (it != NULL) {
        if (it->status == 1) {
            new = (Object*) malloc(sizeof (Object));
            info = (PlayerInfo*) malloc(sizeof (PlayerInfo));
            id++;
            new->id = id;
            new->status = 1;
            new->type = 1;
            new->playerInfo = info;
            new->client = it;

            info->score = 0;
            info->nMegaBombs = 2;
            info->bombs = 3;

            PutPlayer(new, lObjects);
            ult->p = new;
            new->p = NULL;
            ult = new;
        }
        it = it->p;
    }

    for (i = 0; i < 5; i++) {
        new = (Object*) malloc(sizeof (Object));
        id++;
        new->id = id;
        new->status = 1;
        new->type = 2;
        PutEnemys(new, lObjects);
        ult->p = new;
        new->p = NULL;
        ult = new;
    }

    //    nMaxPantano = 5 + (rand() % 10);
    //
    //    for (i = 0; i < nMaxPantano; i++) {
    //        novo = (Objecto*) malloc(sizeof (Objecto));
    //        id++;
    //        novo->id = id;
    //        novo->ativo = 1;
    //        novo->tipo = 6;
    //        ColocaPantano(novo, bjectos);
    //        ult->p = novo;
    //        novo->p = NULL;
    //        ult = novo;
    //        maxElementos++;
    //    }

}

void PutPlayer(Object *new, Object *lObjects) {
    Object *it;
    int x = 2, y = 2, sair = 0;

    srand(time(NULL));

    do {
        it = lObjects;
        while (it != NULL) {
            if (it->x == x && it->y == y) {
                sair = 0;
                break;
            }
            new->x = x;
            new->y = y;
            it = it->p;
            sair = 1;
        }
        y = rand() % 20;
    } while (sair == 0);
}

void PutEnemys(Object *new, Object *lObjects) {
    Object *it;
    int x = 15, y = 2, sair = 0;

    srand(time(NULL));

    do {
        it = lObjects;
        while (it != NULL) {
            if (it->x == x && it->y == y) {
                sair = 0;
                break;
            }
            new->x = x;
            new->y = y;
            it = it->p;
            sair = 1;
        }
        y = rand() % 20;
        x = rand() % 20;
        x += 10;
    } while (sair == 0);
}

void *Game(void *dados) {
    ThreadReferences *x = (ThreadReferences*) dados;
    ThreadReferencesEnemy *d = (ThreadReferencesEnemy*) malloc(sizeof (ThreadReferencesEnemy));
    Play j;
    Object new;
    Object *it;
    Object final;
    int i, fd;
    pthread_t recebe[30];
    int count=0;
    
    final.id = -1;

    d->exit = x->exit;
    d->lClients = x->lClients;
    d->lObjects = x->lObjects;

    mkfifo(FIFO_JOGO, 0660);
    fd = open(FIFO_JOGO, O_RDWR);

    if (fd == -1) {
        printf("Erro ao abrir FIFOJOGO\n");
        fflush(stdout);
        pthread_exit(0);
    }

    if (pthread_mutex_init(&lockFifoJogo, NULL) != 0) {
        printf("\n mutex lockFifoJogo has failed\n");
         pthread_exit(0);
    }

    if (pthread_mutex_init(&modifyList, NULL) != 0) {
        printf("\n mutex modifyList has failed\n");
         pthread_exit(0);
    }

    CreateInitialObjects(x->lObjects, x->lClients);

    it = x->lObjects;
    while (it != NULL) {
        if (it->type == 2) {
            d->enemy = it;
            pthread_create(&recebe[++count], NULL, &MoveEnemy, (void *) d);
        }
        SendAll(*it, x->lClients);
        it = it->p;
    }
    SendAll(final, x->lClients);

    while (*(x->exit) == 0) {
        i = read(fd, &j, sizeof (j));
        if (i == sizeof (j)) {
            ActionPlayer(x->lObjects, j, x->lClients);
        }
    }
    pthread_exit(0);
}

void ActionPlayer(Object *lObjects, Play play, Client *lClients) {

    Object *it;
    int fd, erro = 0, erro2 = 0;
    char str[50];

    pthread_mutex_lock(&modifyList);
    it = lObjects;

    while (it != NULL) {
        if (it->type == 1 && it->client->PID == play.PID) {
            if (CheckMovement(lObjects, it, play) == 1) {
                SendAll(*it, lClients);
                break;
            }
        }
        it = it->p;
    }
    pthread_mutex_unlock(&modifyList);
}

void SendAll(Object new, Client *lClients) {
    char str[50];
    Client *it = lClients;

    int fd;

    while (it != NULL) {
        if (it->status == 1) {
            sprintf(str, "../JJJ%d", it->PID);
            fd = open(str, O_WRONLY);

            if (fd == -1) {
                printf("<ERRO> Nao foi possivel abrir o FIFO <%s>\n", str);
                fflush(stdout);
            } else {

                fflush(stdout);
                pthread_mutex_lock(&lockFifoJogo);
                write(fd, &new, sizeof (new));
                pthread_mutex_unlock(&lockFifoJogo);
                close(fd);
            }
        }
        it = it->p;

    }
}

int CheckMovement(Object *lObjects, Object *object, Play play) {
    Object *it;
    Object new;

    new.x = object->x;
    new.y = object->y;

    if (toupper(play.ascii) == 'W' || play.ascii == 30) {
        new.y--;
    } else {
        if (toupper(play.ascii) == 'S' || play.ascii == 31) {
            new.y++;
        } else {
            if (toupper(play.ascii) == 'D' || play.ascii == 16) {
                new.x++;
            } else {
                if (toupper(play.ascii) == 'A' || play.ascii == 17) {
                    new.x--;
                }
            }
        }
    }

    it = lObjects;

    while (it != NULL) {
        if (it->x == new.x && it->y == new.y) {
            return 0;
        }
        it = it->p;
    }
    object->x = new.x;
    object->y = new.y;
    return 1;
}

void *MoveEnemy(void *dados) {
    ThreadReferencesEnemy *x = (ThreadReferencesEnemy*) dados;
    srand(time(NULL));
    
    int direction = rand() % 4;
    Play play;
    int change = 1;

    srand(time(NULL));

    while (*(x->exit) == 0 && x->enemy->status == 1) {
        if (change == 1) {
            direction = rand() % 4;
            switch (direction) {
                case 0:
                    play.ascii = 'W';
                    break;
                case 1:
                    play.ascii = 'S';
                    break;
                case 2:
                    play.ascii = 'A';
                    break;
                case 3:
                    play.ascii = 'D';
                    break;
            }
        }
        pthread_mutex_lock(&modifyList);
        if (CheckMovement(x->lObjects, x->enemy, play) == 0) {
            change = 1;
        } else {
            change = 0;
        }
        pthread_mutex_unlock(&modifyList);
        SendAll(*(x->enemy),x->lClients);
        sleep(1);
    }
}