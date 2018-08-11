#ifndef SOTOOLS_H
#define SOTOOLS_H

#include "ClientManagement.h"

#ifdef __cplusplus
extern "C" {
#endif

/// RETURMS ARRAY WITH ALL WORD OF A SENTENCE
Word* getWords(char* sentence);

//CLEAN STADIN
void CleanStdin(void);

///SIZE OFF A WORD
int Size(Word *p);

///
char* UpString(char *s);

Word* getWords(char* sentence) {
    Word *p = NULL;
    Word *new = NULL;
    Word *last;
    char str[100];
    strcpy(str, "");

    for (int i = 0; i <= (int) strlen(sentence); i++) {
        if (sentence[i] != ' ' && sentence[i] != '\t' && sentence[i] != '\0') {
            sprintf(str, "%s%c", str, sentence[i]);
        } else {
            if (strlen(str) != 0) {
                if (p == NULL) {
                    p = (Word*) malloc(sizeof (Word));
                    strcpy(p->command, str);
                    p->p = NULL;
                    last = (Word*) p;
                } else {
                    new = (Word*) malloc(sizeof (Word));
                    strcpy(new->command, str);
                    last->p = (struct Word*) new;
                    new->p = NULL;
                    last = (Word*) new;
                }
            }
            strcpy(str, "");
        }
    }
    return p;
}

void CleanStdin(void) {
    int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
}

int Size(Word *p) {
    int count = 0;
    Word *it = p;
    while (it != NULL) {
        count++;
        it = (Word*) it->p;
    }

    return count;
}

char* UpString(char *s) {
    int i = 0;
    while (s[i] != '\0') {
        s[i] = toupper(s[i]);
        i++;
    }
    
    return s;
}
#ifdef __cplusplus
}
#endif

#endif /* SOTOOLS_H */

