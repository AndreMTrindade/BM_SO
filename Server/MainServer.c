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


pthread_mutex_t lockFifoJogo;

Client* Console(Client *clientes);
int IdentifyCommand(Word *p);
Object* ReadMaze();
void *ClientLoginRequest(void *dados);
Object CreateInitialObjects(Object *lObjects, Client *lClients);
void PutPlayer(Object *new, Object *lObjects);
void *Game(void *dados);
void SendAll(Object new, Client *lClients);

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
    int x = 0, y = 0;

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
                        lObjects->type = 2;
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
                        last->p->type = 2;
                        last->p->p = NULL;
                        last = last->p;
                    }
                } else {
                    if (c == 'o') {
                        if (lObjects == NULL) {
                            lObjects = (Object*) malloc(sizeof (Object));
                            lObjects->status = 1;
                            lObjects->id = ++id;
                            lObjects->type = 8;
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
                            last->p->type = 8;
                            last->p->p = NULL;
                            last = last->p;
                        }
                    } else {
                        if (c == 'O') {

                            if (lObjects == NULL) {
                                lObjects = (Object*) malloc(sizeof (Object));
                                lObjects->status = 1;
                                lObjects->id = ++id;
                                lObjects->type = 9;
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
                                last->p->type = 9;
                                last->p->p = NULL;
                                last = last->p;
                            }
                        } else {
                            if (c == 'S') {

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
                    Game(x);
                    //    pthread_create(&envia, NULL, &EnviaDadosJagador, (void *) d);
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

    //    while (itb->p != NULL) {
    //        itb = itb->p;
    //    }
    //    ult = itb;
    //
    //    for (i = 0; i < nMaxEnemy; i++) {
    //        novo = (Objecto*) malloc(sizeof (Objecto));
    //        id++;
    //        novo->id = id;
    //        novo->ativo = 1;
    //        novo->tipo = 7;
    //        ColocaInimigo(novo, bjectos);
    //        ult->p = novo;
    //        novo->p = NULL;
    //        ult = novo;
    //        maxElementos++;
    //    }
    //
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

void *Game(void *dados) {
    ThreadReferences *x = (ThreadReferences*) dados;
    Play j;
    Object new;
    int i, fd;
    pthread_t recebe;

    mkfifo(FIFO_JOGO, 0660);
    fd = open(FIFO_JOGO, O_RDWR);

    if (fd == -1) {
        printf("Erro ao abrir FIFOJOGO\n");
        fflush(stdout);
        pthread_exit(0);
    }

    if (pthread_mutex_init(&lockFifoJogo, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }

    while (*(x->exit) == 0) {
        i = read(fd, &j, sizeof (j));
        if (i == sizeof (j)) {
            ActionPlayer(x->lObjects, j, x->lClients);
        }

    }
    pthread_exit(0);
}

void ActionPlayer(Object *lObjects, Play play, Client *lClients) {

    Object novo;
    Object *it;
    char key;
    int fd, erro = 0, erro2 = 0;
    char str[50];

    it = lObjects;

    while (it != NULL) {

        if (it->type == play.PID) {
            key = play.ascii;
            if (toupper(key) == 'W' || key == 30) {
                novo = VerificaMovimento(1, b, it, c);
                if (novo.y != (it->y - 1)) {
                    EnviaNovopTodos(novo, c);
                    pthread_mutex_unlock(&bloqueiaBomba);
                }

            } else {
                if (toupper(key) == 'S' || key == 31) {
                    novo = VerificaMovimento(2, b, it, c);

                    if (novo.y != (it->y + 1)) {
                        pthread_mutex_lock(&bloqueiaBomba);
                        EnviaNovopTodos(novo, c);
                        pthread_mutex_unlock(&bloqueiaBomba);
                    }

                } else {
                    if (toupper(key) == 'D' || key == 16) {
                        novo = VerificaMovimento(3, b, it, c);

                        if (novo.x != (it->x + 1)) {
                            pthread_mutex_lock(&bloqueiaBomba);
                            EnviaNovopTodos(novo, c);
                            pthread_mutex_unlock(&bloqueiaBomba);
                        }

                    } else {
                        if (toupper(key) == 'A' || key == 17) {
                            novo = VerificaMovimento(4, b, it, c);
                            if (novo.x != (it->x - 1)) {
                                pthread_mutex_lock(&bloqueiaBomba);
                                EnviaNovopTodos(novo, c);
                                pthread_mutex_unlock(&bloqueiaBomba);
                            }
                        } else {
                            if (key == 32) {
                                itc = c;
                                while (itc != NULL) {
                                    if (itc->PID == (j.PID - 10000)) {
                                        if (itc->nBomba > 0) {
                                            itc->nBomba--;
                                            break;
                                        } else {
                                            return;
                                        }

                                    }
                                    itc = itc->p;
                                }
                                erro2 = 0;
                                it2 = b;
                                while (it2->p != NULL) {
                                    erro++;
                                    if (erro == 500) {
                                        it = b;
                                        for (int i = 0; i < maxElementos; i++) {
                                            it = it->p;
                                        }
                                        it->p = NULL;
                                    }
                                    it2 = it2->p;
                                }
                                novaBomba = (Objecto*) malloc(sizeof (Objecto));
                                it2->p = novaBomba;

                                id++;
                                novaBomba->id = id;
                                novaBomba->tipo = 3;
                                novaBomba->explosao = NULL;
                                novaBomba->x = it->x;
                                novaBomba->y = it->y;
                                x->bomba = novaBomba;

                                pthread_create(&envia, NULL, &TrataBomba, (void *) x);
                            } else {
                                if (toupper(key) == 'B') {
                                    itc = c;
                                    while (itc != NULL) {
                                        if (itc->PID == j.PID - 10000) {
                                            if (itc->nMegaBomba > 0) {
                                                itc->nMegaBomba--;
                                            } else {
                                                return;
                                            }

                                        }
                                        itc = itc->p;
                                    }

                                    it2 = b;
                                    erro = 0;

                                    while (it2->p != NULL) {
                                        erro++;
                                        if (erro == 500) {
                                            it = b;
                                            for (int i = 0; i < maxElementos; i++) {
                                                it = it->p;
                                            }
                                            it->p = NULL;
                                        }
                                        it2 = it2->p;
                                    }

                                    novaBomba = (Objecto*) malloc(sizeof (Objecto));
                                    it2->p = novaBomba;

                                    id++;
                                    novaBomba->id = id;
                                    novaBomba->tipo = 4;
                                    novaBomba->explosao = NULL;
                                    novaBomba->x = it->x;
                                    novaBomba->y = it->y;
                                    x->bomba = novaBomba;
                                    pthread_create(&envia, NULL, &TrataMegaBomba, (void *) x);
                                }
                            }
                        }
                    }
                }
            }
            break;
        }
        it = it->p;
    }
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
                pthread_mutex_unlock(&lockFifoJogo)
                close(fd);
            }
        }
        it = it->p;

    }
}

