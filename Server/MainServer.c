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

static int id = 0;

typedef struct Word {
    char command[50];
    struct Word *p;
} Word;



Client* Shell(Client *clientes);
Word* DevolvePalavras(char* frase);
void LimpaStdin(void);
int ProcessaComando(Word *p);
int Size(Word *p);
char* UpString(char *s);

void Shutdown(Client *c);

int main(int argc, char** argv) {

    return (EXIT_SUCCESS);
}



