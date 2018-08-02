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

#ifdef __cplusplus
}
#endif

#endif /* SOTOOLS_H */

