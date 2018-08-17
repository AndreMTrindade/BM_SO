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

typedef struct {
    Client *lClients;
    Object *lObjects;
    Object *player;
    int* exit;
} ThreadReferencesBombs;

typedef struct {
    Client *lClients;
    Object *lObjects;
    Object *bomb;
    int* exit;
} ThreadReferencesBombsAction;

typedef struct {
    Client *lClients;
    Object *player;
    Object *swamp;
} ThreadReferencesSwamp;

pthread_mutex_t lockFifoJogo;
pthread_mutex_t modifyList;

Client* Console(Client *clientes);
int IdentifyCommand(Word *p);
Object* ReadMaze();
void *ClientLoginRequest(void *data);
Object CreateInitialObjects(Object *lObjects, Client *lClients);
void PutPlayer(Object *new, Object *lObjects);
void *Game(void *data);
void SendAll(Object new, Client *lClients);
int ActionPlayer(Object *lObjects, Play play, Client *lClients);
int CheckMovement(Object *lObjects, Object *object, Play play, Client *lClients);
void PutEnemys(Object *new, Object *lObjects);
void *MoveEnemy(void *data);
void *CheckGameOver(void *data);
void *Bomb(void *data);
void CreateFire(Object *lObjects, Object *bomb);
void *CheckDamage(void *data);
void DropItem(Object *lObjects, Object enemy, Client *lClients);
void CathItem(Object *dropObject, Object *lObjects, Object *player, Client *lClients);
void *MegaBomb(void *data);
void CreateMegaFire(Object *lObjects, Object *bomb);
void *CheckSwamp(void *data);
void *Messages(void *data);

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
    Object *find = NULL;

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
            } else {
                if (c == '0') {
                    if (lObjects == NULL) {
                        lObjects = (Object*) malloc(sizeof (Object));
                        lObjects->status = 1;
                        lObjects->id = ++id;
                        lObjects->type = 3;
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
                        last->p->type = 3;
                        last->p->p = NULL;
                        last = last->p;
                    }
                } else {
                    if (c == 'S') {
                        if (lObjects == NULL) {
                            lObjects = (Object*) malloc(sizeof (Object));
                            lObjects->status = 1;
                            lObjects->id = ++id;
                            lObjects->type = 6;
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
                            last->p->type = 6;
                            last->p->p = NULL;
                            last = last->p;
                        }
                    } else {
                        if (c == 'P') {
                            if (lObjects == NULL) {
                                lObjects = (Object*) malloc(sizeof (Object));
                                lObjects->status = 1;
                                lObjects->id = ++id;
                                lObjects->type = 13;
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
                                last->p->type = 13;
                                last->p->p = NULL;
                                last = last->p;
                            }
                            if (find == NULL) {
                                find = last;
                            } else {
                                last->portal = find;
                                find->portal = last;
                                find = NULL;
                            }
                        }
                    }
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

void *ClientLoginRequest(void *data) {
    char str[80];
    int fd, fd_resp, i;
    int res;
    int *Exit;
    int count = 1;

    Client newRequest;

    Object *lObjects;
    ThreadReferences *x = (ThreadReferences*) data;
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
                if (res == 1) {
                    res = count;
                    count++;
                }
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
    Object tempEnemy;
    PlayerInfo info;


    int nMaxPprevano;
    int nMaxEnemy, nMaxObjects;
    int i, x, y;
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
            id++;
            new->id = id;
            new->status = 1;
            new->type = 1;

            new->client = it;

            info.score = 0;
            info.nMegaBombs = 2;
            info.bombs = 3;
            new->playerInfo = info;

            PutPlayer(new, lObjects);
            ult->p = new;
            new->p = NULL;
            ult = new;
        }
        it = it->p;
    }

    for (i = 0; i < 1; i++) {
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

    //    for (i = 0; i < 5; i++) {
    //     PutEnemys(&tempEnemy, lObjects);
    //     DropItem(lObjects, tempEnemy,lClients);
    //    }

    //    nMaxPprevano = 5 + (rand() % 10);
    //
    for (i = 0; i < 5; i++) {
        new = (Object*) malloc(sizeof (Object));
        id++;
        new->id = id;
        new->status = 1;
        new->type = 14;
        PutEnemys(new, lObjects);
        ult->p = new;
        new->p = NULL;
        ult = new;
    }

}

void PutPlayer(Object *new, Object *lObjects) {
    Object *it;
    int x = 2, y = 2, exit = 0;

    srand(time(NULL));

    do {
        it = lObjects;
        while (it != NULL) {
            if (it->x == x && it->y == y) {
                exit = 0;
                break;
            }
            new->x = x;
            new->y = y;
            it = it->p;
            exit = 1;
        }
        y = rand() % 20;
    } while (exit == 0);
}

void PutEnemys(Object *new, Object *lObjects) {
    Object *it;
    int x = 15, y = 2, exit = 0;

    srand(time(NULL));

    do {
        it = lObjects;
        while (it != NULL) {
            if (it->x == x && it->y == y) {
                exit = 0;
                break;
            }
            new->x = x;
            new->y = y;
            it = it->p;
            exit = 1;
        }
        y = rand() % 20;
        x = rand() % 20;
        x += 10;
    } while (exit == 0);
}

void *Game(void *data) {
    ThreadReferences *x = (ThreadReferences*) data;
    ThreadReferencesEnemy *d = (ThreadReferencesEnemy*) malloc(sizeof (ThreadReferencesEnemy));
    Play j;
    Object new;
    Object *it;
    Object final;
    int i, fd;
    pthread_t recebe;
    int count = 0;

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
            d = (ThreadReferencesEnemy*) malloc(sizeof (ThreadReferencesEnemy));
            d->lClients = x->lClients;
            d->lObjects = x->lObjects;
            d->exit = x->exit;
            d->enemy = it;
            pthread_create(&recebe, NULL, &MoveEnemy, (void *) d);
        }
        SendAll(*it, x->lClients);
        it = it->p;
    }
    SendAll(final, x->lClients);

    pthread_create(&recebe, NULL, &CheckGameOver, (void *) x);
    while (*(x->exit) == 0) {
        i = read(fd, &j, sizeof (j));
        if (i == sizeof (j)) {
            if (ActionPlayer(x->lObjects, j, x->lClients) == 1) {
                // *(x->exit) = 1;
            }
        }
    }
    pthread_exit(0);
}

int ActionPlayer(Object *lObjects, Play play, Client *lClients) {

    Object *it;
    int fd, erro = 0, erro2 = 0;
    char str[50];
    int awn;
    Object win;
    pthread_t recebe;
    ThreadReferencesBombs *x = (ThreadReferencesBombs*) malloc(sizeof (ThreadReferencesBombs));

    x->lClients = lClients;
    x->lObjects = lObjects;

    pthread_mutex_lock(&modifyList);
    it = lObjects;

    while (it != NULL) {
        if (it->type == 1 && it->client->PID == play.PID) {
            if (play.ascii != 32 && toupper(play.ascii) != 'B') {
                awn = CheckMovement(lObjects, it, play, x->lClients);
                if (awn > 0) {
                    SendAll(*it, lClients);
                    if (awn == 2) {
                        win.type = -3;
                        SendAll(win, lClients);
                        pthread_mutex_unlock(&modifyList);
                        return 1;
                    }
                    break;
                }
            } else {
                if (play.ascii == 32) {
                    if (it->playerInfo.bombs > 0) {
                        x->player = it;
                        pthread_create(&recebe, NULL, &Bomb, (void *) x);
                        it->playerInfo.bombs--;
                    }
                } else {
                    if (toupper(play.ascii) == 'B') {
                        if (it->playerInfo.nMegaBombs > 0) {
                            x->player = it;
                            pthread_create(&recebe, NULL, &MegaBomb, (void *) x);
                            it->playerInfo.nMegaBombs--;
                        }
                    }
                }
            }
        }
        it = it->p;
    }
    pthread_mutex_unlock(&modifyList);
    return 0;
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

int CheckMovement(Object *lObjects, Object *object, Play play, Client *lClients) {
    Object *it;
    Object new;
    Client *client;
    pthread_t recebe;
    
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
        if (object->type == 2) {
            if (it->x == new.x && it->y == new.y && it->type != 1 && it->status == 1) {
                if (it->type == 6) break;
                return 0;
            }
        } else {
            if (it->x == new.x && it->y == new.y && it->type != 2 && it->status == 1) {
                if (it->type >= 8 && it->type <= 11) {
                    CathItem(it, lObjects, object, lClients);
                    break;
                } else {
                    if (it->type == 13) {
                        if (toupper(play.ascii) == 'W' || play.ascii == 30) {
                            new.x = it->portal->x;
                            new.y = it->portal->y;
                            new.y--;
                        } else {
                            if (toupper(play.ascii) == 'S' || play.ascii == 31) {
                                new.x = it->portal->x;
                                new.y = it->portal->y;
                                new.y++;
                            } else {
                                if (toupper(play.ascii) == 'D' || play.ascii == 16) {
                                    new.x = it->portal->x;
                                    new.y = it->portal->y;
                                    new.x++;
                                } else {
                                    if (toupper(play.ascii) == 'A' || play.ascii == 17) {
                                        new.x = it->portal->x;
                                        new.y = it->portal->y;
                                        new.x--;
                                    }
                                }
                            }
                        }
                        break;
                    } else {
                        if(it->type == 14){
                            ThreadReferencesSwamp *swamp = (ThreadReferencesSwamp*) malloc(sizeof(ThreadReferencesSwamp));
                            swamp->lClients = lClients;
                            swamp->swamp = it;
                            swamp->player = object;
                            pthread_create(&recebe, NULL, &CheckSwamp, (void *) swamp);
                            break;
                        }
                        
                    }
                }
                if (it->type == 6) break;
                if (it->type == 7) {
                    object->client->status = 4;
                    client = lClients;
                    while (client != NULL) {
                        if (client->status == 1) {
                            return 0;
                        }
                        client = client->p;
                    }
                    return 1;
                }
                return 0;
            }
        }
        it = it->p;
    }
    object->x = new.x;
    object->y = new.y;
    return 1;
}

void *MoveEnemy(void *data) {
    ThreadReferencesEnemy *x = (ThreadReferencesEnemy*) data;
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
        if (CheckMovement(x->lObjects, x->enemy, play, x->lClients) == 0) {
            change = 1;
        } else {
            change = 0;
        }
        pthread_mutex_unlock(&modifyList);
        SendAll(*(x->enemy), x->lClients);
        sleep(1);
    }
    pthread_exit(0);
}

void *CheckGameOver(void *data) {
    ThreadReferences *x = (ThreadReferences*) data;
    Object *it = x->lObjects;
    Object *it2 = x->lObjects;
    Object final;
    char str[50];
    int fd;

    final.type = -2;

    while (*(x->exit) == 0) {

        it = x->lObjects;

        pthread_mutex_lock(&modifyList);
        while (it != NULL) {
            if (it->type == 1) {
                it2 = x->lObjects;
                while (it2 != NULL) {
                    if (it->x == it2->x && it->y == it2->y && it2->status == 1) {
                        if (it2->type == 2) {
                            sprintf(str, "../JJJ%d", it->client->PID);
                            fd = open(str, O_WRONLY);
                            if (fd == -1) {
                                printf("<ERRO> Nao foi possivel abrir o FIFO <%s>\n", str);
                                fflush(stdout);
                            } else {
                                pthread_mutex_lock(&lockFifoJogo);
                                write(fd, &final, sizeof (final));
                                pthread_mutex_unlock(&lockFifoJogo);
                                close(fd);
                            }
                        }
                    }
                    it2 = it2->p;
                }
            }
            it = it->p;
        }
        pthread_mutex_unlock(&modifyList);
    }
}

void *Bomb(void *data) {
    ThreadReferencesBombs *x = (ThreadReferencesBombs*) data;
    ThreadReferencesBombsAction *d = (ThreadReferencesBombsAction*) malloc(sizeof (ThreadReferencesBombsAction));
    pthread_t recebe;
    int exit;

    Object *it;
    Object *temp;

    //CREATE BOMB
    Object *bomb = (Object*) malloc(sizeof (Object));

    bomb->id = ++id;
    bomb->type = 4;
    bomb->status = 1;
    bomb->p = NULL;
    bomb->explosion = NULL;
    bomb->x = x->player->x;
    bomb->y = x->player->y;

    //SEND BOMB
    SendAll(*bomb, x->lClients);

    exit = 0;
    d->bomb = bomb;
    d->exit = &exit;
    d->lClients = x->lClients;
    d->lObjects = x->lObjects;

    sleep(3);

    bomb->status = 0;
    SendAll(*bomb, x->lClients);

    CreateFire(x->lObjects, bomb);
    it = bomb->explosion;
    if (bomb->explosion != NULL) {
        while (it != NULL) {
            SendAll(*it, x->lClients);
            it = it->p;
        }
    }

    pthread_create(&recebe, NULL, &CheckDamage, (void *) d);

    sleep(1.5);

    it = bomb->explosion;
    if (bomb->explosion != NULL) {
        while (it != NULL) {
            it->status = 0;
            SendAll(*it, x->lClients);
            it = it->p;
        }
    }
    exit = 1;
    it = bomb->explosion;
    if (bomb->explosion != NULL) {
        while (it != NULL) {
            temp = it;
            it = it->p;
            if (it != NULL)
                free(temp);
        }
    }
    free(bomb);
    free(x);
    pthread_exit(0);
}

void CreateFire(Object *lObjects, Object *bomb) {
    Object *it = bomb;
    Object *itb = lObjects;
    Object temp, *new;

    int find = 0;
    int destruc = 0;
    temp = *bomb;

    id++;
    new = (Object*) malloc(sizeof (Object));
    new->status = 1;
    new->id = id;
    new->p = NULL;
    new->type = 5;
    new->x = temp.x;
    new->y = temp.y;

    it->explosion = new;
    it = new;

    for (int i = 0; i < 4; i++) {
        temp = *bomb;
        destruc = 0;
        for (int j = 0; j < 2; j++) {
            if (i == 0) {
                temp.y++;
            } else if (i == 1) {
                temp.y--;
            } else if (i == 2) {
                temp.x++;
            } else if (i == 3) {
                temp.x--;
            }

            find = 1;
            itb = lObjects;
            pthread_mutex_lock(&modifyList);
            while (itb != NULL) {
                if (itb->x == temp.x && itb->y == temp.y && (itb->type == 0 || destruc == 1)) {
                    find = 0;
                    j = 2;
                    break;
                } else {
                    if (itb->x == temp.x && itb->y == temp.y && itb->type == 3) {
                        destruc = 1;
                        break;
                    }
                }
                itb = itb->p;
            }
            pthread_mutex_unlock(&modifyList);
            if (find == 1) {
                id++;
                new = (Object*) malloc(sizeof (Object));
                new->type = 1;
                new->id = id;
                new->p = NULL;
                new->type = 5;
                new->x = temp.x;
                new->y = temp.y;

                it->p = new;
                it = new;
            }
        }
    }

}

void *CheckDamage(void *data) {
    ThreadReferencesBombsAction *x = (ThreadReferencesBombsAction*) data;
    Object *it, *itb;
    Object final;
    char str[50];
    int fd;
    final.type = -2;

    while (*(x->exit) == 0) {
        itb = x->bomb->explosion;
        while (itb != NULL) {
            pthread_mutex_lock(&modifyList);
            it = x->lObjects;
            while (it != NULL) {
                if (itb->x == it->x && itb->y == it->y && it->status == 1) {
                    if (it->type == 1) {
                        it->status = 0;
                        sprintf(str, "../JJJ%d", it->client->PID);
                        it->client->status = 0;
                        fd = open(str, O_WRONLY);

                        if (fd == -1) {
                            printf("<ERRO> Nao foi possivel abrir o FIFO <%s>\n", str);
                            fflush(stdout);
                        } else {
                            sleep(0.3);
                            pthread_mutex_lock(&lockFifoJogo);
                            write(fd, &final, sizeof (final));
                            pthread_mutex_unlock(&lockFifoJogo);
                            close(fd);
                        }
                    } else {
                        if (it->type == 3) {
                            it->status = 0;
                            SendAll(*it, x->lClients);
                        } else {
                            if (it->type == 2) {
                                it->status = 0;
                                SendAll(*it, x->lClients);
                                DropItem(x->lObjects, *it, x->lClients);
                            }
                        }
                    }
                }
                it = it->p;
            }
            pthread_mutex_unlock(&modifyList);
            itb = itb->p;
        }

    }
    pthread_exit(0);
}

void DropItem(Object *lObjects, Object enemy, Client *lClients) {
    Object *it, *new;
    int temp;

    srand(time(NULL));
    int prob = rand() % 101;


    if (prob > 0 && prob <= 10) {
        temp = 11;
    } else if (prob > 10 && prob <= 35) {
        temp = 10;
    } else if (prob > 35 && prob <= 55) {
        temp = 9;
    } else if (prob > 55) {
        temp = 8;
    }

    it = lObjects;
    while (it->p != NULL) {
        it = it->p;
    }

    new = (Object*) malloc(sizeof (Object));
    it->p = new;

    id++;
    new->id = id;
    new->type = temp;
    new->status = 1;
    new->explosion = NULL;
    new->x = enemy.x;
    new->y = enemy.y;

    SendAll(*new, lClients);

}

void CathItem(Object *dropObject, Object *lObjects, Object *player, Client *lClients) {
    Object *it;
    Object *exit;
    int count = 0;


    if (dropObject->type == 8) {
        player->playerInfo.bombs++;
        player->playerInfo.score++;
    } else {
        if (dropObject->type == 9) {
            player->playerInfo.nMegaBombs++;
            player->playerInfo.score++;

        } else if (dropObject->type == 11) {
            player->playerInfo.score += 10;

        } else if (dropObject->type == 10) {
            it = lObjects;
            while (it != NULL) {
                if (it->type >= 8 && it->type <= 9) {
                    switch (it->type) {
                        case 8:
                            player->playerInfo.bombs++;
                            player->playerInfo.score++;
                            break;
                        case 9:
                            player->playerInfo.nMegaBombs++;
                            player->playerInfo.score++;
                            break;
                        case 10:
                            ///COLETOS AUTOMATICO
                            player->playerInfo.score++;
                            break;
                        case 11:
                            player->playerInfo.score += 10;
                            break;
                    }
                    count++;
                    it->status = 0;
                    SendAll(*it, lClients);
                }
                if (count == 5) {
                    break;
                }

                it = it->p;
            }
        }
    }

    SendAll(*player, lClients);

    dropObject->status = 0;
    SendAll(*dropObject, lClients);

    count = 0;
    it = lObjects;
    while (it != NULL) {
        if ((it->type == 2 || it->type >= 8 && it->type <= 11) && it->status == 1) {
            count = 1;
        }
        if (it->type == 6) {
            exit = it;
        }
        it = it->p;
    }

    if (count == 0) {
        exit->type = 7;
        SendAll(*exit, lClients);
    }



}

void *MegaBomb(void *data) {
    ThreadReferencesBombs *x = (ThreadReferencesBombs*) data;
    ThreadReferencesBombsAction *d = (ThreadReferencesBombsAction*) malloc(sizeof (ThreadReferencesBombsAction));
    pthread_t recebe;
    int exit;

    Object *it;
    Object *temp;

    //CREATE BOMB
    Object *bomb = (Object*) malloc(sizeof (Object));

    bomb->id = ++id;
    bomb->type = 12;
    bomb->status = 1;
    bomb->p = NULL;
    bomb->explosion = NULL;
    bomb->x = x->player->x;
    bomb->y = x->player->y;

    //SEND BOMB
    SendAll(*bomb, x->lClients);

    exit = 0;
    d->bomb = bomb;
    d->exit = &exit;
    d->lClients = x->lClients;
    d->lObjects = x->lObjects;

    sleep(3);

    bomb->status = 0;
    SendAll(*bomb, x->lClients);

    CreateMegaFire(x->lObjects, bomb);
    it = bomb->explosion;
    if (bomb->explosion != NULL) {
        while (it != NULL) {
            SendAll(*it, x->lClients);
            it = it->p;
        }
    }

    pthread_create(&recebe, NULL, &CheckDamage, (void *) d);

    sleep(1.5);

    it = bomb->explosion;
    if (bomb->explosion != NULL) {
        while (it != NULL) {
            it->status = 0;
            SendAll(*it, x->lClients);
            it = it->p;
        }
    }
    exit = 1;
    it = bomb->explosion;
    if (bomb->explosion != NULL) {
        while (it != NULL) {
            temp = it;
            it = it->p;
            if (it != NULL)
                free(temp);
        }
    }
    free(bomb);
    free(x);
    pthread_exit(0);
}

void CreateMegaFire(Object *lObjects, Object *bomb) {
    Object *it = bomb;
    Object *itb = lObjects;
    Object temp, *new;

    int find = 0;
    int destruc = 0;
    temp = *bomb;

    id++;
    new = (Object*) malloc(sizeof (Object));
    new->status = 1;
    new->id = id;
    new->p = NULL;
    new->type = 5;
    new->x = temp.x;
    new->y = temp.y;

    it->explosion = new;
    it = new;

    for (int i = 0; i < 4; i++) {
        temp = *bomb;
        destruc = 0;
        for (int j = 0; j < 4; j++) {
            if (i == 0) {
                temp.y++;
            } else if (i == 1) {
                temp.y--;
            } else if (i == 2) {
                temp.x++;
            } else if (i == 3) {
                temp.x--;
            }

            find = 1;
            itb = lObjects;
            pthread_mutex_lock(&modifyList);
            while (itb != NULL) {
                if (itb->x == temp.x && itb->y == temp.y && (itb->type == 0 || destruc == 1)) {
                    find = 0;
                    j = 4;
                    break;
                } else {
                    if (itb->x == temp.x && itb->y == temp.y && itb->type == 3) {
                        destruc = 1;
                        break;
                    }
                }
                itb = itb->p;
            }
            pthread_mutex_unlock(&modifyList);
            if (find == 1) {
                id++;
                new = (Object*) malloc(sizeof (Object));
                new->type = 1;
                new->id = id;
                new->p = NULL;
                new->type = 5;
                new->x = temp.x;
                new->y = temp.y;

                it->p = new;
                it = new;
            }
        }
    }

}

void *CheckSwamp(void *data) {
    ThreadReferencesSwamp *x = (ThreadReferencesSwamp*) data;
    int i;
    int fd;
    char str[50];
    Object final;
    final.type = -2;

    for (i = 0; i < 3; i++) {
        if (x->player->x != x->swamp->x || x->player->y != x->swamp->y) {
            pthread_exit(0);
        }
        sleep(1);
    }
    if (x->player->x == x->swamp->x && x->player->y == x->swamp->y) {
        x->player->client->status = 0;
        sprintf(str, "../JJJ%d", x->player->client->PID);
        x->player->client->status = 0;
        fd = open(str, O_WRONLY);

        if (fd == -1) {
            printf("<ERRO> Nao foi possivel abrir o FIFO <%s>\n", str);
            fflush(stdout);
        } else {
            sleep(0.3);
            pthread_mutex_lock(&lockFifoJogo);
            write(fd, &final, sizeof (final));
            pthread_mutex_unlock(&lockFifoJogo);
            close(fd);
        }
    }
    pthread_exit(0);
}

void *Messages(void *data){
    ThreadReferences *x = (ThreadReferences*)data;
    int fd;
    mkfifo(FIFO_MESSAGES, 0660);
    fd = open(FIFO_MESSAGES, O_RDWR);
    
    while (*(x->exit) == 0) {
        i = read(fd, &j, sizeof (j));
        if (i == sizeof (j)) {
            if (ActionPlayer(x->lObjects, j, x->lClients) == 1) {
                // *(x->exit) = 1;
            }
        }
    }
    pthread_exit(0);
}
