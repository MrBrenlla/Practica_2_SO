#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/shm.h>
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
  //Engadese o nodo no caso da lista vacía
  if (l->inicio==NULL){
    l->inicio=malloc(sizeof(tNodoM));
    if(l->inicio!=NULL){
      *(l->inicio)=nodo;
      l->final=l->inicio;
    }
    else perror("Error");
  }
  //Engadese o nodo no resto dos casos
  else{
    l->final->sig = malloc(sizeof(tNodoM));
    if(l->final->sig!=NULL){
      l->final=l->final->sig;
      *(l->final)=nodo;
    }
    else perror("Error");
  }
}
/*
--------------------------------------------------------------------------------
*/
void * borrarNodo(tListM * l, void * dato, int (*comp)(tNodoM a, void * b)){
  tNodoM * aux = l->inicio;
  tNodoM * aux2;
  void * tmp;
  //Borrar nodo se solo hai un elemento
  if (aux!=NULL){
    if(comp(*aux,dato)){
      l->inicio = l->inicio->sig;
      tmp=aux->addr;
      if (aux==l->final) l->final=NULL;
      //liberase a memoria reservada para o asignar correspondente a o nodo a borrar dependendo do tipo
      if(strcmp(aux->type,"malloc")==0) free(aux->addr);
      if(strcmp(aux->type,"mmap")==0) if(munmap(aux->addr,aux->size)!=0){perror("Error");return NULL;}
      if(strcmp(aux->type,"shared")==0) if(shmdt(aux->addr)!=0){perror("Error");return NULL;};

      free(aux);
      return tmp;
    }
    //Borrar nodo se hai mais de un elemento
    else while(aux!=NULL){
      aux2=aux->sig;
      if(aux2!=NULL && (comp(*aux2,dato))){
        aux->sig = aux2->sig;
        tmp=aux2->addr;
        if (aux2==l->final) l->final=aux;
        //liberase a memoria reservada para o asignar correspondente a o nodo a borrar dependendo do tipo
        if(strcmp(aux2->type,"malloc")==0) free(aux2->addr);
        if(strcmp(aux->type,"mmap")==0) if(munmap(aux2->addr,aux->size)!=0){perror("Error");return NULL;}
        if(strcmp(aux->type,"shared")==0) if(shmdt(aux2->addr)!=0){perror("Error");return NULL;};
        free(aux2);
        return tmp;
      }
      else aux=aux2;
    }
  }
  return NULL;
}
/*
--------------------------------------------------------------------------------
*/
void vaciar(tListM * l){
  //Vaciase a lista e desasignanse todas as direccións reservadas
  tNodoM * aux;
  aux = l->inicio;
  if (aux != NULL){
    l->inicio=l->inicio->sig;

    if(strcmp(aux->type,"malloc")==0) free(aux->addr);
    if(strcmp(aux->type,"mmap")==0) if(munmap(aux->addr,aux->size)!=0){perror("Error");return NULL;}
    if(strcmp(aux->type,"shared")==0) if(shmdt(aux->addr)!=0){perror("Error");return NULL;};
    free(aux);
    vaciar(l);
  }
  else l->final=NULL;
}
/*
--------------------------------------------------------------------------------
*/
void verListaM(tListM l, char type[]){
  //Mostranse os nodos do tipo indicado ou todos se se recive "all"
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
  }
  aux=aux->sig;
}

}
/*
--------------------------------------------------------------------------------
*/
//Funcións de comparación de diversos campos usados no borrarNodo

int equalMalloc(tNodoM a,void * b){
  return (a.size==*((int*)b) && strcmp(a.type,"malloc")==0);
}
int equalMmap(tNodoM a,void * b){
  return (strcmp(a.nomF,((char *)b))==0 && strcmp(a.type,"mmap")==0);
}
int equalShared(tNodoM a,void * b){
  return (a.cl_fd==*((int *)b) && strcmp(a.type,"shared")==0);
}
int equalAddr(tNodoM a,void * b){
  return (a.addr==b);
}
/*
--------------------------------------------------------------------------------
*/
