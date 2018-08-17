#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <ncurses.h>

#include "EstruturasComunicacao.h"

typedef struct {
    Object *lObjects;
    int *Exit;
    WINDOW * mainwin;
} ThreadReferences;

Client* InitialData(Client *c);
int SendLoginData(Client c);
Object* ReciveInitialObjects();
void* ReciveCurrentData(void *dados);
void Show(Object *ob);
void CleanStdin(void);

int player;

int main(int argc, char** argv) {

    Client c;
    char str[50];
    int fd, fde;
    int Sair = 0;
    char tecla;
    Object *ob;
    Play j;
    ThreadReferences *passadadosThread = (ThreadReferences*) malloc(sizeof (ThreadReferences));
    pthread_t envia;

    WINDOW * mainwin;

    do {
        clear;
        InitialData(&c);
    } while (SendLoginData(c) == -1);

    ob = ReciveInitialObjects();

    passadadosThread->Exit = &Sair;
    passadadosThread->lObjects = ob;
    passadadosThread->mainwin = mainwin;

    if ((mainwin = initscr()) == NULL) {
        fprintf(stderr, "Error initialising ncurses.\n");
        exit(EXIT_FAILURE);
    }

    noecho();
    keypad(mainwin, TRUE);
    curs_set(0);
    start_color();
    Show(ob);

    pthread_create(&envia, NULL, &ReciveCurrentData, (void *) passadadosThread);

    do {
        scanf("%c", &tecla);
        fde = open(FIFO_JOGO, O_WRONLY);
        if (fde == -1) {
            printf("Erro ao Abrir FIFO \n");
            fflush(stdout);
        } else {
            j.PID = getpid();
            j.ascii = (int) tecla;
            write(fde, &j, sizeof (j));
            close(fde);
        }

    } while (Sair == 0);

    delwin(mainwin);
    endwin();
    refresh();
    return (EXIT_SUCCESS);
    return (EXIT_SUCCESS);
}

void CleanStdin(void) {
    int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
}

Client* InitialData(Client *c) {
    char name[50];
    char pass[50];

    do {
        printf("Nome: ");
        scanf("%[^\n]", name);
        CleanStdin();
    } while (strlen(name) <= 0);


    do {
        printf("Palavra-Chave: ");
        scanf("%[^\n]", pass);
        CleanStdin();
    } while (strlen(pass) < 4);

    strcpy(c->name, name);
    strcpy(c->passWord, pass);
    c->PID = getpid();

    return c;
}

int SendLoginData(Client c) {
    char str[20];
    int fdres;
    int res;
    int fd = open(FIFO_LOGIN, O_WRONLY);

    sprintf(str, "../JJJ%d", c.PID);
    mkfifo(str, 0600);
    write(fd, &c, sizeof (Client));
    close(fd);
    printf("Esperando Resposta do Servidor...\n");
    fflush(stdout);

    fdres = open(str, O_RDONLY);

    if (fdres == -1) {
        printf("Erro ao criar fifo JJJ\n");
    }

    read(fdres, &res, sizeof (res));
    fflush(stdout);
    close(fdres);


    if (res == 0) {
        printf("%s Jogador já está online!\n", c.name);
        sleep(3);
        return -1;
    } else {
        if (res >= 1) {
            printf("Login efectuado com sucesso!\n\n E o Jogador %d\n", res);
            player = res;
            return 1;
        } else {
            printf("Jogador inexistente!\n");
            sleep(3);
            return -1;
        }
    }
}

Object* ReciveInitialObjects() {
    int fd, i;
    char str[50];
    Object *lObjets = NULL;
    Object *ul;
    Object b;

    sprintf(str, "../JJJ%d", getpid());

    fd = open(str, O_RDONLY);
    if (fd == -1) {
        printf("<ERRO> Erro ao abrir o Ficheiro FIFO\n");
        fflush(stdout);
        pthread_exit(0);
    }

    while (1) {
        i = read(fd, &b, sizeof (b));
        printf("Recebeu: %d\n", b.id);
        fflush(stdout);
        if (i == sizeof (b)) {
            if (b.id == -1) {
                break;
            }
            if (lObjets == NULL) {
                lObjets = (Object*) malloc(sizeof (b));
                lObjets->status = b.status;
                lObjets->id = b.id;
                lObjets->type = b.type;
                lObjets->x = b.x;
                lObjets->y = b.y;
                lObjets->p = NULL;
                ul = lObjets;
            } else {
                ul->p = (Object*) malloc(sizeof (b));
                ul->p->status = b.status;
                ul->p->id = b.id;
                ul->p->type = b.type;
                ul->p->x = b.x;
                ul->p->y = b.y;
                ul->p->p = NULL;
                ul = ul->p;
            }
        }
    }
    close(fd);
    return lObjets;
}

void* ReciveCurrentData(void *dados) {
    ThreadReferences *x = (ThreadReferences*) dados;
    char str[50];
    int nplayer;
    int fd, i, existe = 0;
    Object *it, *temp, *ant = NULL;
    Object b;

    sprintf(str, "../JJJ%d", getpid());

    fd = open(str, O_RDWR);
    if (fd == -1) {
        printf("<ERRO> Erro ao abrir o Ficheiro FIFO\n");
        fflush(stdout);
        pthread_exit(0);
    }

    while (*(x->Exit) == 0) {
        i = read(fd, &b, sizeof (b));
        if (i == sizeof (b)) {
            it = x->lObjects;
            if (b.id == -5) {
                endwin();
                refresh();
                printf("O seu jogador foi kicado!\n");
                sleep(2);
                pthread_exit(0);
            } else {
                if (b.type == -1) {
                    clear();
                    delwin(x->mainwin);
                    endwin();
                    refresh();
                    printf("O Servidor foi terminado!\n");
                    fflush(stdout);
                    sleep(2);
                    pthread_exit(0);
                } else {
                    if (b.type == -2) {
                        clear();
                        delwin(x->mainwin);
                        endwin();
                        refresh();
                        printf("Perdeu!\n");
                        fflush(stdout);
                        sleep(2);
                        pthread_exit(0);
                    } else {
                        if (b.type == -3) {
                            nplayer = 1;
                            clear();
//                            init_pair(1, COLOR_WHITE, COLOR_BLACK);
//
//                            attron(COLOR_PAIR(1));
                            mvprintw(10, 10, "Jogo Terminou");
                            it = x->lObjects;
                            while (it != NULL) {
                                if (it->type == 1) {
                                    mvprintw(1 + nplayer, 40, "Jogador %d : %d", nplayer, it->playerInfo.score);
                                    nplayer++;
                                }
                                it = it->p;
                            }
                          //  sleep(5);
                            getch();
                            clear();
                            delwin(x->mainwin);
                            endwin();
                            refresh();
                            *(x->Exit) = 1;
                            pthread_exit(0);
                        }
                    }
                }

            }
            existe = 0;
            while (it != NULL) {
                if (it->id == b.id) {
                    existe = 1;

                    if (b.status == 0) {
                        if (ant == NULL) {
                            x->lObjects = x->lObjects->p;
                        } else {
                            ant->p = it->p;
                            free(it);

                        }
                    } else {
                        it->x = b.x;
                        it->y = b.y;
                        if (b.type == 1) {
                            it->playerInfo = b.playerInfo;
                        } else if (b.type == 7) {
                            it->type = b.type;
                        }
                    }
                    break;
                }
                ant = it;
                it = it->p;
            }
            if (existe == 0) {
                it = x->lObjects;
                while (it->p != NULL) {
                    it = it->p;
                }
                temp = (Object*) malloc(sizeof (Object));
                it->p = temp;
                temp->id = b.id;
                temp->status = b.status;
                temp->type = b.type;
                temp->x = b.x;
                temp->y = b.y;
                temp->p = NULL;
            }
            Show(x->lObjects);
        }
    }
    pthread_exit(0);
}

void Show(Object *ob) {

    int nplayer = 0;
    Object *it;
    it = ob;
    clear();

    init_pair(1, COLOR_WHITE, COLOR_WHITE);
    init_pair(2, COLOR_WHITE, COLOR_WHITE);
    init_pair(3, COLOR_BLACK, COLOR_RED);
    init_pair(5, COLOR_YELLOW, COLOR_YELLOW);
    init_pair(6, COLOR_BLUE, COLOR_BLUE);
    init_pair(7, COLOR_RED, COLOR_BLACK);
    init_pair(8, COLOR_BLACK, COLOR_YELLOW);

    init_pair(9, COLOR_WHITE, COLOR_BLACK);
    init_pair(10, COLOR_YELLOW, COLOR_BLACK);
    init_pair(11, COLOR_RED, COLOR_BLACK);
    init_pair(12, COLOR_CYAN, COLOR_BLACK);
    init_pair(13, COLOR_MAGENTA, COLOR_MAGENTA);
    init_pair(14, COLOR_BLUE, COLOR_BLACK);
    init_pair(15, COLOR_WHITE, COLOR_BLACK);
    init_pair(16, COLOR_RED, COLOR_RED);
    init_pair(17, COLOR_WHITE, COLOR_GREEN);
    init_pair(18, COLOR_CYAN, COLOR_CYAN);
    init_pair(19, COLOR_GREEN, COLOR_GREEN);

    while (it != NULL) {
        if (it->type == 0) {
            attron(COLOR_PAIR(1));
            mvprintw(it->y, it->x, "0");
        } else {
            if (it->type == 1) {
                attron(COLOR_PAIR(3));
                mvprintw(it->y, it->x, "P");
                nplayer++;
                attron(COLOR_PAIR(9));
                mvprintw(1 + nplayer, 40, "Jogador %d : %d", nplayer, it->playerInfo.score);
                if (player == nplayer) {
                    mvprintw(20, 40, "Bombas %d  Mega Bombas %d", it->playerInfo.bombs, it->playerInfo.nMegaBombs);
                }
            } else if (it->type == 2) {
                attron(COLOR_PAIR(7));
                mvprintw(it->y, it->x, "E");
            } else if (it->type == 3) {
                attron(COLOR_PAIR(6));
                mvprintw(it->y, it->x, "M");
            } else if (it->type == 4) {
                attron(COLOR_PAIR(7));
                mvprintw(it->y, it->x, "x");
            } else if (it->type == 5) {
                attron(COLOR_PAIR(5));
                mvprintw(it->y, it->x, "M");
            } else if (it->type == 6) {
                attron(COLOR_PAIR(9));
                mvprintw(it->y, it->x, "S");
            } else if (it->type == 7) {
                attron(COLOR_PAIR(17));
                mvprintw(it->y, it->x, "S");
            } else if (it->type == 8) {
                attron(COLOR_PAIR(12));
                mvprintw(it->y, it->x, "o");
            } else if (it->type == 9) {
                attron(COLOR_PAIR(12));
                mvprintw(it->y, it->x, "O");
            } else if (it->type == 10) {
                attron(COLOR_PAIR(12));
                mvprintw(it->y, it->x, "C");
            } else if (it->type == 11) {
                attron(COLOR_PAIR(12));
                mvprintw(it->y, it->x, "p");
            } else if (it->type == 12) {
                attron(COLOR_PAIR(7));
                mvprintw(it->y, it->x, "X");
            } else if(it->type == 13) {
                attron(COLOR_PAIR(18));
                mvprintw(it->y, it->x, "P");
            }  else if(it->type == 14) {
                attron(COLOR_PAIR(19));
                mvprintw(it->y, it->x, "p");
            } 
        }
        it = it->p;
    }
    refresh();

}
