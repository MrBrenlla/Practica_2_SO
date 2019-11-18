#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "list.h"

/*
--------------------------------------------------------------------------------
*/
tNodoM crearNodo(void * addr, int size, char type[15], int cl_fd, char nomF[MAX]){
  tNodoM n;
  n.addr=addr;
  n.size=size;
  strcpy(n.type,type);
  n.cl_fd=cl_fd;
  strcpy(n.nomF,nomF);
  n.time=time(0);
  n.sig=NULL;
  return n;
}
/*
--------------------------------------------------------------------------------
*/
void addListM(tListM * l, void * addr, int size, char type[15], int cl_fd, char nomF[MAX]){
  tNodoM nodo = crearNodo(addr,size,type,cl_fd,nomF);
  if (l->inicio==NULL){
    l->inicio=malloc(sizeof(tNodoM));
    if(l->inicio!=NULL){
      *(l->inicio)=nodo;
      l->final=l->inicio;
    }
    else perror("Error");
  }
  else{
    l->final->sig = malloc(sizeof(tNodoM));
    if(l->final->sig!=NULL){
      l->final=l->final->sig;
      *(l->final)=nodo;
    }
    else perror("Error");
  }
};
/*
--------------------------------------------------------------------------------
*/
int borrarNodo(tListM * l, void * dato, int (*comp)(tNodoM a, void * b)){
  tNodoM * aux = l->inicio;
  tNodoM * aux2;
  if (aux!=NULL){
    if(comp(*aux,dato)){
      l->inicio = l->inicio->sig;
      free(aux->addr);
      free(aux);
      return 0;
    }
    else while(aux!=NULL){
      aux2=aux->sig;
      if(aux2!=NULL && (comp(*aux2,dato))){
        aux->sig = aux2->sig;
        free(aux2->addr);
        free(aux2);
        return 0;
      }
      else aux=aux2;
    }
  }
  return 1;
};
/*
--------------------------------------------------------------------------------
*/
void vaciar(tListM * l){
  tNodoM * aux;
  aux = l->inicio;
  if (aux != NULL){
    l->inicio=l->inicio->sig;
    free(aux->addr);
    free(aux);
    vaciar(l);
  }
  else l->final=NULL;
};
/*
--------------------------------------------------------------------------------
*/
void verListaM(tListM l, char type[]){
tNodoM * aux = l.inicio;
while (aux!=NULL){
  if (strcmp(aux->type,type)==0 || strcmp(type,"all")==0){
    printf("%p: size:%7d. %7s %10s ",aux->addr,aux->size,aux->type,aux->nomF);
    if (strcmp(aux->type,"mmap")==0) printf("(fd:%5d) ", aux->cl_fd );
    else if (strcmp(aux->type,"shared")==0) printf("(key %5d) ", aux->cl_fd );
    struct tm time = *(localtime(&(aux->time)));
    char output[MAX];
    strftime(output,MAX,"%c",&time);
    printf("%s\n",output);
    aux=aux->sig;
  }
}



};
/*
--------------------------------------------------------------------------------
*/
int equalMalloc(tNodoM a,void * b){
  return (a.size==*((int*)b) && strcmp(a.type,"malloc")==0);
};
int equalMmap(tNodoM a,void * b){
  return (strcmp(a.nomF,((char *)b))==0 && strcmp(a.type,"mmap")==0);
};
int equalShared(tNodoM a,void * b){
  return (a.cl_fd==*((int *)b) && strcmp(a.type,"shared")==0);
};
int equalAddr(tNodoM a,void * b){
  return strcmp(a.addr,((char* )b));
};
/*
--------------------------------------------------------------------------------
*/
