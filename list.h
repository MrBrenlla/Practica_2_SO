#include <time.h>


#define MAX  300


typedef struct nodoM{
  void * addr;
  int size ;
  time_t time ;
  char type[MAX];
  int cl_fd ;
  char nomF[MAX] ;
  struct nodoM * sig ;
} tNodoM;

typedef struct listM {
  tNodoM * inicio;
  tNodoM * final;
} tListM;



tNodoM crearNodo(void * addr, int size, char type[], int cl_fd , char nomF[MAX]);
void addListM(tListM * l, void * addr, int size, char type[15], int cl_fd, char nomF[MAX]);
void * borrarNodo(tListM * l, void * dato, int (*comp)(tNodoM a, void * b));
void vaciar(tListM * l);
void verListaM(tListM l, char type[]);
int equalMalloc(tNodoM a,void * b);
int equalMmap(tNodoM a,void * b);
int equalShared(tNodoM a,void * b);
int equalAddr(tNodoM a,void * b);
