
#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

#ifdef __cplusplus
extern "C" {
#endif

    ///FIFOS
#define FIFO_LOGIN "../LOGIN"
#define FIFO_JOGO "../GAME"
#define FIFO_MESSAGES "../SMS"

typedef struct Client{
   char name[50];
   char passWord[50];
   int PID;
   int status;
   struct Client *p;
}Client;    

typedef struct PlayerInfo{
    int nMegaBombs;
    int bombs;
    int score;
}PlayerInfo;
 
typedef struct Object{
    int id;
    int x,y;
    int status;
    int type; /// 0 - Labirinto  ___ 1 - Jogador ___ 2 - Inimigo ___ 3 - Labirinto Destrutivel ____ 4 - Bomba __ 5 - Fogo ___ 6 - Saida
    
    PlayerInfo playerInfo;
    struct Object *explosion;
    Client *client;
    struct Object *portal;

    struct Object *p;
}Object;

typedef struct Message{
    char message;
    char sender;
} Message;


typedef struct Play {
    char ascii;
    int PID;
} Play;



#ifdef __cplusplus
}
#endif

#endif /* ESTRUTURAS_H */

