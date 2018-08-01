/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SOTools.h
 * Author: hp
 *
 * Created on 1 de Agosto de 2018, 18:34
 */

#ifndef SOTOOLS_H
#define SOTOOLS_H

//#include "../EstruturasComunicacao.h"

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

