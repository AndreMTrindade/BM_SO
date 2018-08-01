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


Client* Console(Client *clientes);
int IdentifyCommand(Word *p);
Object* ReadMaze();
void *ClientLoginRequest(void *dados);

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
                    last->p->type = 1;
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