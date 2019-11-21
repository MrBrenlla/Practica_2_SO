
// Practica 2 se SO
//Parte 1, shell.c
// Data: 21 de Novembro do 2019

// Grupo 1.3
// Brais García Brenlla ; b.brenlla
// Ángel Álvarez Rey ; angel.alvarez.rey



//Inclúense varias librerías necesarias para a realización dos comandos
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include "list.h"
#include <stddef.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define LEERCOMPLETO ((ssize_t)-1)

int aa, bb , cc;

typedef  struct nodo {
 	char dato[MAX];
	struct nodo * sig;
 } tNodo;

typedef struct list{
	tNodo * inicio;
	tNodo * final;
} tList;

//Lista de función utilizadas
void limpiarBuffer(char buf[]);
int TrocearCadena(char  cadena[], char cad1[] , char cad2[]);
void fin(int * x );
long tamanho(char nom[]);
char * gid_to_string(gid_t id);
char * uid_to_string(uid_t id);
void autores( char arg[], int palabras);
void fecha();
void hora();
void cdir(char arg[], int palabras);
void pid(char arg[], int palabras);
void gardar(char teclado[MAX],tList * h);
void verhist(tList h);
void borrarhist(tList * h);
void hist(char arg[], tList * h, int palabras);
void crear(char arg[], int palabras);
char TipoFichero (mode_t m);
char * ConvierteModo (mode_t m);
void auxInfo(char dir[],char arg[]);
void info(char arg[],int palabras);
void mostrar(int l,int v,struct dirent * sig, char dir[]);
void auxListar(char actualdir[], char aux1[], int rec, int l, int r, int v);
void listar(char arg[]);
void borrar (char arg[], int palabras);
void * MmapFichero (char * fichero, int protection,tListM * m);
void Cmd_AsignarMmap (char *arg[],tListM * m);
ssize_t LeerFichero (char *fich, void *p, ssize_t n);
int TrocearCadenaEnArray(char * cadena,char * trozos[]);
void asignar (char arg[], int palabras,tListM * m);
void escollerFuncion(char com[],char arg[],int palabras,int * acabado,tList * h,tListM * m);

/*
--------------------------------------------------------------------------------
*/
void limpiarBuffer(char buf[]){
  //Póñense todos os campos da cadea a '\0', o carácter nulo
	for (int i  = 0 ; i<MAX ; i++ ){
		buf[i]='\0';
	}
}
/*
--------------------------------------------------------------------------------
*/
int TrocearCadena(char  cadena[], char cad1[] , char cad2[]){
	int espacios =0;
	int palabras =0;
	int letras =1;
	char trozos[MAX];

	limpiarBuffer(trozos);
  //Elimínanse os espacios innecesarios ou engádese un ao final se non hai,
  //de forma que a nova cadea quede formada por palabras seguidas de espacios
	for (int i=0; (cadena[i]!='\0')&&(cadena[i]!='\n')  ; i++){
		if (cadena[i]==' '){
			espacios+=1;
		}
		else{
			trozos[i-espacios]=cadena[i];
			if( (cadena[i+1]==' ') || (cadena[i+1]=='\0') || (cadena[i+1]=='\n' )){
        espacios-=1;
        trozos[(i-espacios)]=' ';
			}
		}
	}

  //Usando a nova cadea cóllense a primeira palabra como cad1 e e o resto como cad2
  // e úsanse os espazos para contar o número de palabras
  limpiarBuffer(cad1);
  limpiarBuffer(cad2);
  int ult;
	for (int i=0 ; (cadena[i]!='\0') ; i++ ) {
		if ((trozos[i]==' ')){
			palabras+=1;
		}
			switch (palabras) {
				case 0: cad1[i]=trozos[i]; letras+=1; break;
				default:cad2[i-(letras)]=trozos[i];ult= i-(letras); break;
		}
	}
  //Ao haberen casos que utilizan cad2 como nombre de archivos eliminaselle
  // o espazo final
  if (palabras==2) cad2[ult]='\0';

return palabras;
}
/*
--------------------------------------------------------------------------------
*/
void fin(int * x ){
	*x=1;
}
/*
--------------------------------------------------------------------------------
*/
long tamanho(char nom[]){
  //Dado o nome de un archivo do directrio actual devolve o tamaño, ou no seu defecto
  // 0 se houbo un erro
  struct stat datos;
  if(lstat(nom,&datos)==0){
    return datos.st_size;
  }
  else perror("Error");
  return 0;
}
/*
--------------------------------------------------------------------------------
*/
char * gid_to_string(gid_t id){
  struct group *grp;
  grp = getgrgid(id);
  if (grp == NULL) {
    perror("getgrgid");
    return "";
  }
  return grp->gr_name;
}
/*
--------------------------------------------------------------------------------
*/
char * uid_to_string(uid_t id){
  //Dado o id dun usuario devolvese o nome deste ou no seu defecto "" se houbo un erro
  struct passwd *pwd;
  pwd = getpwuid(id);
  if(pwd == NULL) {
      perror("getpwuid");
      return "";
  }
  return pwd->pw_name;
}
/*
--------------------------------------------------------------------------------
*/
void autores( char arg[], int palabras){
    //Se só se escribiu comando imprímese toda a información de autores
		if (palabras == 1){
				printf("Brais: b.brenlla\nÁngel: angel.alvarez.rey\n");
		}
    //Se se ecribe argumento mírase que parte se debe decir, ou no seu defecto, "Error"
		else if (palabras == 2){
				if (strncmp(arg,"-l\0",3)==0){
						printf("b.brenlla\nangel.alvarez.rey\n");
				} else if (strncmp(arg,"-n\0",3)==0){
						 printf("Brais\nÁngel\n");
					}
					else{
						printf("Argumento no válido\n");
					}
		}
    else {
      printf("Demasiados argumentos\n" );
    }

}
/*
--------------------------------------------------------------------------------
*/
void fecha(){
	time_t tiempo = time(0);
        //Pídese a fecha ao sistema e móstrase por pantalla
        struct tm tlocal = *(localtime(&tiempo));
        char output[MAX];
        strftime(output,MAX,"%c",&tlocal);
        printf("%s\n",output);
}
/*
--------------------------------------------------------------------------------
*/
void hora(){
	time_t tiempo = time(0);
        //Pídese a fecha ao sistema e móstrase só a hora por pantalla
        struct tm tlocal = *(localtime(&tiempo));
        char output[128];
        strftime(output,128,"%H:%M:%S",&tlocal);
        printf("%s\n",output);

}
/*
--------------------------------------------------------------------------------
*/
void cdir(char arg[], int palabras){
	char dir[MAX];
	limpiarBuffer(dir);
  //Se só se puxo o comando pídese ao sistema a dirección actual e móstrase por pantalla
	if (palabras==1){
		getcwd(dir,MAX);
		puts(dir);
	}
  //Se hai argumento diráselle ao sistema a que dirección cambiar, ou no seu defecto, saltará "Error"
	if ((palabras==2) && (chdir(arg)!=0)){
				printf("Error\n" );
		}
  if (palabras>2) printf("Demasiados argumentos\n" );
	}
/*
--------------------------------------------------------------------------------
*/
void pid(char arg[], int palabras) {
  pid_t pid = getpid();
  pid_t pidp = getppid();

  if (palabras>2) printf("Demasiados argumentos\n" );
  else if (pid != 0) {
    //Se só se escribiu o comando pídese o pid do shell ao sistema e móstrase por pantalla
    if (palabras == 1) {
      printf("Pid del shell: %u\n",pid);
      //Se só se escribiu o argumento "-p" pídese o pid do pai do shell ao sistema e móstrase por pantalla
    }
     else if ((palabras==2)&&(!strcmp("-p",arg))){
      printf("Pid del padre del shell: %u\n",pidp);
      //En outro caso móstrase que o argumento non é válido
    } else {
      printf("Argumento no válido\n");
    }

  }
}
/*
--------------------------------------------------------------------------------
*/
void gardar(char teclado[MAX],tList * h){
  //Se a lista está vacía inicialízase cun novo nodo e cárganse aí os datos
	if (h->inicio==NULL){
		h->inicio=malloc(sizeof(tNodo));
    if (h->inicio!=NULL){ //comprobación do malloc
  		h->final=h->inicio;
  		strcpy(h->inicio->dato,teclado);
  		h->inicio->sig=NULL;
    }
    else printf("Error ao asignar espacio de memoria a un punteiro\n" );
	}
  //Se xa está inicializada créase un novo nodo ao final, gárdase o dato e modifícase final
	else{
		h->final->sig=malloc(sizeof(tNodo));
    if (h->final->sig!=NULL){ //comprobación do malloc
    		h->final=h->final->sig;
    		strcpy(h->final->dato,teclado);
    		h->final->sig=NULL;
      }
      else printf("Error ao asignar espacio de memoria a un punteiro\n" );
	}
}
/*
--------------------------------------------------------------------------------
*/
void verhist(tList h){
	tNodo * aux;
	aux=h.inicio;
  //Recórrese a lista mostrando os elementos por pantalla
	while(aux!=NULL){
		printf("%s",aux->dato );
		aux=aux->sig;
	}
}
/*
--------------------------------------------------------------------------------
*/
void borrarhist(tList * h){
  tNodo * aux;
  aux = h->inicio;
  //Se a lista non está vacía elimínaase o primeiro elemento
  //e vólvese a chamar a borrarhist(h) de forma recursiva ata valeirala
  if (aux != NULL){
    h->inicio=h->inicio->sig;
    free(aux);
    borrarhist(h);
  }
  else h->final=NULL;

}
/*
--------------------------------------------------------------------------------
*/
void hist(char arg[], tList * h, int palabras){
  //Se só se escribiu o comando móstrase por pantalla a lista
  if (palabras==1){
    verhist(*h);
  }
  if (palabras==2){
    //Se se reciviu o argumento -c bórrase o historial
    if (strncmp(arg,"-c\0",3)==0){
      borrarhist(h);
    }
    //En outro caso salta o erro "Argumento no válido"
    else{
      printf("Argumento no válido\n" );
    }
  }
  if (palabras>2) printf("Demasiados argumentos\n" );
}
/*
--------------------------------------------------------------------------------
*/
void crear(char arg[], int palabras){
  //Se non hai argumentos listase ./
  if (palabras == 1){
    listar("");
  }
  if (palabras==2){
    //Se o unico argumento é -d listase ./
    if (strncmp(arg,"-d\0",3)==0) listar("");
    else{ //En caso de ser outro intentase crear
      FILE *file;
      file=fopen(arg,"r");
      if (file==NULL) file=fopen(arg,"a");
      else printf("Error: Fichero ya existente\n"); //Se existe salta erro
      fclose(file);
    }

  }
  if (palabras == 3){
    //Se hai dous argumentos e o primeiro é -d intentase crear o directorio
    if (strncmp(arg,"-d ",3)==0){
      char aux1[MAX], aux2[MAX],aux3[MAX];
      limpiarBuffer(aux3);
      TrocearCadena(arg, aux1, aux2); //Quedámonos cas duas ultimas palabras
      getcwd(aux1, MAX);
      strcat(strcat(strcat(aux3,aux1),"/"),aux2);
      if(chdir(aux3)!=0) mkdir(aux3, 0700);
      else{
        printf("El directoria ya existe\n" ); //Se existe salta erro
        chdir(aux1);
      }
     }
   else {
     printf("Argumento no valido\n"); //Se o primeiro non é -d salta un erro
   }
 }
 if (palabras>3) printf("Demasiados argumentos\n" );
}
/*
--------------------------------------------------------------------------------
*/
char TipoFichero (mode_t m){
switch (m&S_IFMT) { /*and bit a bit con los bits de formato,0170000 */
case S_IFSOCK: return 's'; /*socket */
case S_IFLNK: return 'l'; /*symbolic link*/
case S_IFREG: return '-'; /* fichero normal*/
case S_IFBLK: return 'b'; /*block device*/
case S_IFDIR: return 'd'; /*directorio */
case S_IFCHR: return 'c'; /*char device*/
case S_IFIFO: return 'p'; /*pipe*/
default: return '?'; /*desconocido, no deberia aparecer*/
}
}
/*
--------------------------------------------------------------------------------
*/
char * ConvierteModo (mode_t m){
char * permisos;
permisos=(char *) malloc (12);
strcpy (permisos,"---------- ");
permisos[0]=TipoFichero(m);
if (m&S_IRUSR) permisos[1]='r'; /*propietario*/
if (m&S_IWUSR) permisos[2]='w';
if (m&S_IXUSR) permisos[3]='x';
if (m&S_IRGRP) permisos[4]='r'; /*grupo*/
if (m&S_IWGRP) permisos[5]='w';
if (m&S_IXGRP) permisos[6]='x';
if (m&S_IROTH) permisos[7]='r'; /*resto*/
if (m&S_IWOTH) permisos[8]='w';
if (m&S_IXOTH) permisos[9]='x';
if (m&S_ISUID) permisos[3]='s'; /*setuid, setgid y stickybit*/
if (m&S_ISGID) permisos[6]='s';
if (m&S_ISVTX) permisos[9]='t';
return (permisos);
}
/*
--------------------------------------------------------------------------------
*/
void auxInfo(char dir[], char arg[]){
  //Recívese un nome e a ruta que fixo falta para chegar ata el para mostrar a
  //sua información ao igual que o comando "ls -li"  de linux
  struct tm time ;
  char aux1[128], nom[MAX];
  char * aux2 ;
  if(strncmp("\0",arg,1)==0) strcpy(arg,".");
  struct stat datos;
  if(lstat(arg,&datos)==0){
    aux2 = ConvierteModo(datos.st_mode);
    printf("%7ld %s %2ld ",datos.st_ino,aux2,datos.st_nlink );
    printf("%7s %7s %7ld ",uid_to_string(datos.st_uid),gid_to_string(datos.st_gid),datos.st_size);
    time=*(localtime(&datos.st_mtime));
    strftime(aux1,128,"%d %b %H:%M",&time);
    printf("%s ",aux1);
    limpiarBuffer(nom);
    limpiarBuffer(aux1);
    //Mirase se é un link para así poder indicar a que dirixe en caso de selo
    if(*aux2=='l'){
      readlink(arg,aux1,MAX);
      strcat(strcat(nom," -> "),aux1);
    }
    if (strncmp(arg,"./",2)!=0 && strncmp(arg,".\0",2)!=0 && strncmp(arg,"../",3)!=0){
      printf("%s/%s%s\n",dir,arg, nom );
    }
    else printf("%s/%s\n",dir,nom );
    free(aux2);
  }
  else perror(" Error");//Se o nome non existe salta un erro
}
/*
--------------------------------------------------------------------------------
*/
void info(char arg[],int palabras){
  //Procesanse os argumentos para ir chamando a "auxInfo()" con cada un dos ficheiros
  //dos que solicitou
  char aux1[MAX], aux2[MAX], aux3[MAX];
  strcpy(aux2,arg);
    limpiarBuffer(aux1);
    palabras=TrocearCadena(aux2,aux1,aux3);
    limpiarBuffer(aux2);
    strcpy(aux2,aux3);
  while(palabras>0){
    auxInfo(".",aux1);
    palabras=TrocearCadena(aux2,aux1,aux3);
    limpiarBuffer(aux2);
    strcpy(aux2,aux3);
  }
  auxInfo(".",aux1);
}
/*
--------------------------------------------------------------------------------
*/
void mostrar(int l,int v,struct dirent * sig , char dir[]){
  //Elixese si se debe mostrar o ficheiro que recive e en caso afirmativo
  //cal é o formato adecuado
  char nom[MAX];
  strcpy(nom,sig->d_name);
  if (v==0 || strncmp(nom,".",1)!=0){
    if (l==0) printf("%s %ld\n",nom, tamanho(nom));
    else auxInfo(dir,nom);
  }
}
/*
--------------------------------------------------------------------------------
*/
void auxListar(char actualdir[], char aux1[], int rec, int l, int r, int v){
 char dir[MAX], tmp[MAX];
 int comp;

 //En caso de que non nos pasen arquivo consideraremos . o directorio a listar
  if (strncmp(aux1,"\0\0",2)==0){
    strcpy(aux1,".");
    comp=0;
  }
  else{ //En outro caso comprobase que sexa un directorio valido
    limpiarBuffer(dir);
    getcwd(dir,MAX);
    comp = chdir(aux1);
    if(comp==0)
      if(strncmp(aux1,".\0",2)!=0){
         chdir(dir);
        }
  }
  if(rec==0) printf("********%s\n",actualdir );

  //Se o arquivo existe procedese a leer os ficheiros e directorios que contén e a listar
  if(comp==0){
    if (rec==1) printf("********%s\n",actualdir );
    DIR * direct;
    direct = opendir(aux1);
    struct dirent * sig = readdir(direct);
    chdir(aux1);
    while(sig!=NULL){
      mostrar(l,v,sig,actualdir);
      sig=readdir(direct);
    }
    //Se o flag r está activado calculase o actualdir de cada iteración da
    //recursividade e realizase o listar de cada arquivo e directorio listados
    if(r==1 || rec==1){
      rewinddir(direct);
      sig=readdir(direct);
      while(sig!=NULL){
        if(strncmp(sig->d_name,".\0",2)!=0 && strncmp(sig->d_name,"..\0",3)!=0){
          limpiarBuffer(tmp);
          strcat(strcat(strcat(tmp,actualdir),"/"),sig->d_name);
          auxListar(tmp,sig->d_name,1,l,r,v);
        }
        sig=readdir(direct);
      }
    }
    closedir(direct);
    chdir(dir);
  }
  //Se o arquivo introducido polo usuario (os seus recursivos non) non é un
  //directorio slata un erro
  else if(rec==0) perror("Error");
}
/*
--------------------------------------------------------------------------------
*/
void listar(char arg[]){
  int palabras;
  int l =0;
  int v =0;
  int r =0;
  int comp =0;
  char aux1[MAX], aux2[MAX], aux3[MAX];
  strcpy(aux2,arg);
  //Comprabanse os argumentos por orde ata o primeiro que non sexa -r , -l ou -v
  // para activar os flags necesarios
  while (comp==0){
    palabras=TrocearCadena(aux2,aux1,aux3);
    if (strncmp(aux1,"-l\0",3)==0) l=1;
    else if (strncmp(aux1,"-v\0",3)==0) v=1;
          else if (strncmp(aux1,"-r\0",3)==0) r=1;
            else comp=1;
    limpiarBuffer(aux2);
    strcpy(aux2,aux3);
  }
  getcwd(aux3,MAX);
  //A partir de primeira palbra diferente de r , -l ou -v usase a funccion
  //auxListar() para mostrar por pantalla os datos dos directorios
  while(palabras>0){
    if(strncmp(aux1,"..",2)!=0) auxListar(aux1,aux1, 0, l, r, v);
      else{
        if(chdir("..")==0){
          auxListar(aux1,&aux1[1], 0, l, r, v);
          chdir(aux3);
        }
        else perror("Error");
      }
    palabras=TrocearCadena(aux2,aux1,aux3);
    limpiarBuffer(aux2);
    strcpy(aux2,aux3);
  }
  if(strncmp(aux1,"..",2)!=0) auxListar(aux1,aux1, 0, l, r, v);
    else{
      if(chdir("..")==0){
        auxListar(aux1,&aux1[1], 0, l, r, v);
        chdir(aux3);
      }
      else perror("Error");
    }
}
/*
--------------------------------------------------------------------------------
*/
void borrar (char arg[], int palabras){
  if (palabras==1){
    //Se non se poñen argumentos faise un listar de .
    char dir[MAX];
    limpiarBuffer(dir);
    listar(".");
  }

  if (palabras==2){
    //Se se introduce un solo argumento diferente de -r intentase borrar o ficheiro
    //ou directorio con ese nome
    if (strncmp(arg,"-r\0 ",3)!=0){
      //Se non se pode borrar salta error
        if (remove(arg) != 0) perror("Borrar no ha sido posible");
    }
    //Se o único argumento é -r faise un listar de .
    else{
      char dir[MAX];
    limpiarBuffer(dir);
    listar(".");
    }
  }

  if (palabras == 3){
    //Se hai dous argumentos comprobase que o primeiro sexa -r e se o é borrase de forma recursiva
    char aux1[MAX] , aux2[MAX];
    limpiarBuffer(aux1);
    limpiarBuffer(aux2);
    TrocearCadena(arg,aux1,aux2);
    char d[MAX];
    getcwd(d,MAX);
    if (strncmp(aux1,"-r\0",3)==0){
      int f;
      f=remove(aux2); //Intentamos borrar o arquivo ou directorio
      if(f!=0){ //Se non se pode borrar (polo que é un directorio non vacio) faise borrar -r de todo o que ten dentro
        int comp = chdir(aux2);
        if(comp==0){
          chdir("..");
          DIR * direct;
          direct = opendir(aux2);
          struct dirent * sig = readdir(direct);
          chdir(aux2);
            while(sig!=NULL){
              if(strncmp(sig->d_name,".\0",2)!=0 && strncmp(sig->d_name,"..\0",3)!=0){
                limpiarBuffer(aux1);
                borrar(strcat(strcat(aux1,"-r "),sig->d_name),3);
              }
              sig=readdir(direct);
              f=0;
            }
            chdir(d);
            closedir(direct);
            borrar(aux2,2);
        }
        //Se despois do borrado recursivo segue se poder borrarse salte erro
        if(f!=0){perror("Borrar no ha sido posible");}
      }
    }
    //Se o primeiro argumento non é -r salta erro
    else{printf("Argumento no valido\n");}
  }
  if (palabras>3) printf("Demasiados argumentos\n" );
}
/*
--------------------------------------------------------------------------------
*/
void * MmapFichero (char * fichero, int protection, tListM * m){
    int df, map=MAP_PRIVATE,modo=O_RDONLY;

    struct stat s;
    void *p;
    if (protection&PROT_WRITE)
        modo=O_RDWR;
    if (stat(fichero,&s)==-1 || (df=open(fichero, modo))==-1)
        return NULL;
    if ((p=mmap (NULL,s.st_size, protection,map,df,0))==MAP_FAILED) return NULL;
    else addListM(m,p, s.st_size, "mmap", df, fichero);
    return p;
}
/*
--------------------------------------------------------------------------------
*/
void Cmd_AsignarMmap (char *arg[],tListM * m){
    char *perm;
    void *p;
    int protection=0;
    if ((perm=arg[1])!=NULL && strlen(perm)<4) {
        if (strchr(perm,'r')!=NULL) protection|=PROT_READ;
        if (strchr(perm,'w')!=NULL) protection|=PROT_WRITE;
        if (strchr(perm,'x')!=NULL) protection|=PROT_EXEC;
    }
    if ((p=MmapFichero(arg[0],protection,m))==NULL)
        perror ("Imposible mapear fichero");
    else
      printf ("fichero %s mapeado en %p\n", arg[0], p);
    }
/*
--------------------------------------------------------------------------------
*/
int TrocearCadenaEnArray(char * cadena, char * trozos[]){
  int i=1;
    if ((trozos[0]=strtok(cadena," \n\t"))==NULL)
    return 0;
    while ((trozos[i]=strtok(NULL," \n\t"))!=NULL)
    i++;
    return i;
}
/*
--------------------------------------------------------------------------------
*/
void * ObtenerMemoriaShmget (key_t clave, size_t tam,tListM * m){
  void * p;
  int aux,id,flags=0777;
  struct shmid_ds s;
  if (tam) /*si tam no es 0 la crea en modo exclusivo */
    flags=flags | IPC_CREAT | IPC_EXCL;
          /*si tam es 0 intenta acceder a una ya creada*/
  if (clave==IPC_PRIVATE)
    /*no nos vale*/
    {errno=EINVAL; return NULL;}
  if ((id=shmget(clave, tam, flags))==-1)
    return (NULL);
  if ((p=shmat(id,NULL,0))==(void*) -1){
      aux=errno;
      /*si se ha creado y no se puede mapear*/
        if (tam) /*se borra */
            shmctl(id,IPC_RMID,NULL);
      errno=aux;
      return (NULL);
  }
  shmctl (id,IPC_STAT,&s);
  addListM(m,p, tam, "shared", clave, "");
  return (p);
}
/*
--------------------------------------------------------------------------------
*/
void Cmd_AsignarCreateShared (char *arg[],tListM * m){
  key_t k;
  size_t tam=0;
  void *p;
  if (arg[0]==NULL || arg[1]==NULL){
    verListaM(*m,"shared"); return;
  }
  k=(key_t) atoi(arg[0]);
  if (arg[1]!=NULL)
    tam=(size_t) atoll(arg[1]);
  if ((p=ObtenerMemoriaShmget(k,tam,m))==NULL)
    perror ("Imposible obtener memoria shmget");
  else
    printf ("Memoria de shmget de clave %d asignada en %p\n",k,p);
}
/*
--------------------------------------------------------------------------------
*/
void asignar (char arg[], int palabras, tListM * m){

  if (palabras == 1){
    verListaM(*m,"all");
  }
  //Se só se introduce 1 argumento lístase en funcion da palabra introducida
  if (palabras == 2){
      if (strncmp(arg,"-malloc",7)==0) verListaM(*m,"malloc");
      else if (strncmp(arg,"-mmap",5)==0) verListaM(*m,"mmap");
      else if (strncmp(arg,"-crearshared",12)==0) verListaM(*m,"shared");
      else if (strncmp(arg,"-shared",7)==0) verListaM(*m,"shared");
      else printf("uso: allocate [-malloc|-shared|-createshared|-mmap]");
  }
  //Se se introducen 2 argumentos
  if (palabras == 3){
      if (strncmp(arg,"-malloc",7)==0) {
        //Se se introduciu asignar -malloc _tam_
          char aux1[MAX], *aux2, aux3[MAX];
          TrocearCadena(arg, aux1, aux3); //Gardamos en aux3 o argumento _tam_
          int tam = atoi(aux3); //Convertimos a integer o string
          aux2 = malloc(tam);
          //Comprobamos se a memoria se reservou correctamente
          if (NULL != aux2){
            addListM(m,aux2, tam, "malloc", 0, "");
            printf("allocated %s at %p\n",aux3, aux2);
          }
          else {
            //non se reservou correctamente
            printf("Erro ao reservar memoria\n");
          }
      }
      if(strncmp(arg,"-mmap",5)==0){
        //Se se introduciu asignar -mmap fich
        char * troceado[MAX];
        TrocearCadenaEnArray(arg,troceado);
         Cmd_AsignarMmap(&troceado[1],m);
       }
      if (strncmp(arg,"-shared",7)==0) {
        //Se se introduciu asignar -shared _cl_
        char aux1[MAX],aux2[MAX];
        void *p;
        TrocearCadena(arg,aux1,aux2);
        key_t clave = atoi(aux2);

        if ((p = ObtenerMemoriaShmget(clave,0,m)) == NULL)
          printf("Non se puido asignar: No such file or directory\n");
        else
          printf("Memoria de shmget de clave %d asignada en %p\n",clave,p);
      }

      if (strncmp(arg,"-crearshared",12)==0) {
        //Se se introduciu asignar -crearshared _cl_
        char * troceado[MAX];
        TrocearCadenaEnArray(arg,troceado);
        Cmd_AsignarCreateShared(&troceado[1],m);
      }
  }

  //Se se introducen 3 argumentos
  if (palabras == 4){
    if (strncmp(arg,"-mmap",5)==0) {
      //Se se introduciu asignar -mmap fich _perm_
      char * troceado[MAX];
      TrocearCadenaEnArray(arg,troceado);
       Cmd_AsignarMmap(&troceado[1],m);
    }
    if (strncmp(arg,"-crearshared",12)==0) {
      //Se se introduciu asignar -crearshared _cl_ _tam_
      char * troceado[MAX];
      TrocearCadenaEnArray(arg,troceado);
      Cmd_AsignarCreateShared(&troceado[1],m);
    }
  }
}
/*
--------------------------------------------------------------------------------
*/
void desasignar (char arg[], int palabras, tListM * m){
  char aux1[MAX],aux2[MAX];
  void * tmp;
  if (palabras == 1){
    verListaM(*m,"all");
  }

  //Se só se introduce 1 argumento lístase en funcion da palabra introducida
  if (palabras == 2){
      if (strncmp(arg,"-malloc",7)==0) verListaM(*m,"malloc");
      else if (strncmp(arg,"-mmap",5)==0) verListaM(*m,"mmap");

      else if (strncmp(arg,"-shared",7)==0) verListaM(*m,"shared");
      else {
        void * dir = (void *)strtol(arg, NULL, 16);
        dir = borrarNodo(m,dir,equalAddr);
        if(dir==NULL) printf("Dirección non valida\n");
        else printf("block at address at %p deallocated\n",dir);
      }
  }
  //Se se introducen 2 argumentos
  if (palabras == 3){
    TrocearCadena(arg,aux1,aux2);
      if (strncmp(aux1,"-malloc",7)==0) {

        //Se se introduciu asignar -malloc _tam_
        int  num = atoi(aux2);
        tmp = borrarNodo(m,(void *) &num, equalMalloc);
        if(tmp==NULL)
          printf("Desasignar non foi posíbel\n" );
          else{
            printf("Desasignados %s en %p\n",aux2,tmp);
          }
      }
      if (strncmp(aux1,"-mmap",5)==0) {
        //Se se introduciu desasignar -mmap fich
        tmp=borrarNodo(m,(void *) aux2, equalMmap);
        if(tmp==NULL)
          printf("Desmapear non foi posíbel\n" );
        else
          printf("Desmapeados %s en %p\n",aux2,tmp);
      }
      if (strncmp(aux1,"-shared",7)==0) {
        int  num = atoi(aux2);
        //Se se introduciu desasignar -shared _cl_
        tmp=borrarNodo(m,(void *) &num, equalShared);
        if(tmp==NULL)
          printf("Non foi posíbel separar a memoria compartida con clave %s\n", aux2);
        else
          printf("Separouse o bloque de memoria con clave %s en %p\n",aux2,tmp);
      }
  }
}
/*
--------------------------------------------------------------------------------
*/
void volcar(char arg[], int palabras){
  //mostrase por pantalla os valores da dirección indicada
  char aux1[MAX],aux2[MAX];
  unsigned char c;
  int max = 25;
  if(palabras==1) printf("Falta addr\n");
  else if(palabras==2 || palabras == 3){
    TrocearCadena(arg,aux1,aux2);
    //Se se puxo cantas posicións se mostran, e se non queda como 25
    if (palabras==3) max = atoi(aux2);
    //Transformase o string ao punteiro da direccion
    char * dir = (void *)strtol(aux1, NULL, 16);
    for(int j=0; j<max; j+=25){ //Liñas de 25
      for(int i =0 ; i<25 && i+j<max ; i++){//Codigo hexadecimal
        c = *(dir+i+j);
        printf("%2.2x ", c);
      }
      printf("\n" );
      for(int i =0 ; i<25 && i+j<max ; i++){//Caracter
        c = *(dir+i+j);
        printf("%2c ", c);
      }
      printf("\n" );
    }

  }
  else printf("Demasiados argumentos\n");
}
/*
--------------------------------------------------------------------------------
*/
void llenar(char arg[], int palabras){
  //Enchese a dirección recivida tantas posición indicadas co char que se recive (en codigo ou char)
  char aux1[MAX],aux2[MAX],aux3[MAX];
  char c=65;
  int max = 25;
  if(palabras==1) printf("Falta addr\n");
  else if(palabras==2 || palabras==3 || palabras==4){
    TrocearCadena(arg,aux1,aux2);
    char * dir = (void *)strtol(aux1, NULL, 16);//Consiguese a direccion
    if(palabras==3)max=atoi(aux2);//Se hai collese o numero de posicións, se non é 25
    else if(palabras==4){
      TrocearCadena(aux2,aux1,aux3);//Se hai numero de posición e caracter a escribir
      max=atoi(aux1);//collese o numero de posicións, se non é 25
      if(aux3[0]=='\'' && aux3[2]=='\'' && aux3[3]=='\0') c=aux3[1];//Caracter entre comillas
      else c=(char)strtol(aux3, NULL, 0);//Codigo ascii (hexadecimal [0x] ou decimal)
    }
    for(int i=0; i<max;i++) *(dir+i)=c;
  }
  else printf("Demasiados argumentos\n");

}
/*
--------------------------------------------------------------------------------
*/
void Cmd_borrakey (char *args[]){
  key_t clave;
  int id;
  char *key=args[0];
  if (key==NULL || (clave=(key_t) strtoul(key,NULL,10))==IPC_PRIVATE){
    printf ("rmkey clave_valida\n");
    return;
  }
  if ((id=shmget(clave,0,0666))==-1){
    perror ("shmget: imposible obtener memoria compartida");return;
  }
  if (shmctl(id,IPC_RMID,NULL)==-1)
    perror ("shmctl: imposible eliminar memoria compartida\n");
}
/*
--------------------------------------------------------------------------------
*/
void recursiva (int n)
{
char automatico[2048];
static char estatico[2048];
printf ("parametro n:%d en %p\n",n,&n);
printf ("array estatico en:%p \n",estatico);
printf ("array automatico en %p\n",automatico);
n--;
if (n>=0)
recursiva(n);
}
/*
--------------------------------------------------------------------------------
*/
void borrarkey(char arg[], int palabras){
  char *troc[MAX];
  TrocearCadenaEnArray(arg,troc);
  Cmd_borrakey(troc);
}
 /*
--------------------------------------------------------------------------------
*/
void rfich(char arg[],int palabras){
  if (palabras<3) printf("Faltan argumentos\n");
  else if(palabras>4) printf("Demasiados argumentos\n");
  else{
    ssize_t max = LEERCOMPLETO;
    char * troc[MAX];
    TrocearCadenaEnArray(arg,troc);
    void * dir = (void *)strtol(troc[1], NULL, 16);
    if(troc[2]!=NULL) max = atoi(troc[2]);
    LeerFichero(troc[0],dir,max);
  }
}
/*
--------------------------------------------------------------------------------
*/
ssize_t LeerFichero (char *fich, void *p, ssize_t n) /*n=-1 indica que se lea todo*/
{
ssize_t nleidos,tam=n;
int df, aux;
struct stat s;
if (stat (fich,&s)==-1 || (df=open(fich,O_RDONLY))==-1)
return ((ssize_t)-1);
if (n==LEERCOMPLETO)
tam=(ssize_t) s.st_size;
if ((nleidos=read(df,p, tam))==-1){
aux=errno;
close(df);
errno=aux;
return ((ssize_t)-1);
}
close (df);
return (nleidos);
}
/*
--------------------------------------------------------------------------------
*/
void wfich(char arg[],int palabras){
  if (palabras<3) printf("Faltan argumentos\n");
  else if(palabras>5) printf("Demasiados argumentos\n");
  else{
    unsigned int max;
    char * troc[MAX];
    TrocearCadenaEnArray(arg,troc);
    int h;
    if(strncmp("-o\n",troc[0],3)==0) h=1; else h=0;
    if(h==0 && palabras==5){printf("Hai un argumento demais ou no orde incorrecto\n");return;}
    int f = -1;
    if(h==0) f = open(troc[0],O_WRONLY| O_APPEND | O_CREAT, S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH);
    if (f==-1) f = open(troc[h],O_WRONLY | O_APPEND | O_CREAT | O_EXCL, S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH);
    if (f==-1) {perror("Error" );return;}
    char * dir = (void *)strtol(troc[1+h], NULL, 16);
    max = atoi(troc[2+h]);
    if(write(f,dir,max)==-1){close(f);perror("Error");}
    else close(f);
  }
}
/*
--------------------------------------------------------------------------------
*/
void mem(char arg[], int palabras, tListM m){
int a, b, c;
  if (palabras ==1){
    printf("Variables locales %p, %p, %p\n",&a,&b,&c);
    printf("Variables globales %p, %p, %p\n",&aa,&bb,&cc);
    printf("Funciones programa %p, %p, %p\n",mem,wfich,rfich);
  }
  else if (palabras==2){
    if (strncmp(arg,"-malloc",7)==0) verListaM(m,"malloc");
    else if (strncmp(arg,"-mmap",5)==0) verListaM(m,"mmap");
    else if (strncmp(arg,"-shared",7)==0) verListaM(m,"shared");
    else if (strncmp(arg,"-all",4)==0) verListaM(m,"all");
    else printf("uso: mem [-malloc|-shared|-mmap]");
  }
}
/*
--------------------------------------------------------------------------------
*/
void escollerFuncion(char com[],char arg[],int palabras,int * acabado,tList * h,tListM * m){
  //Tendo en conta o comando recivido e o número de palabras chámase a función necesaria
  // ou salta o erro por comando non válido ou por exceso de argumentos
		if (strncmp(com,"autores\0",8)==0){
			autores(arg,palabras);
		}
		else{
			if (strncmp(com,"pid\0",4)==0){
				pid(arg,palabras);
			}
			else{
				if (strncmp(com,"cdir\0",5)==0){
					cdir(arg,palabras);
				}
				else{
					if (strncmp(com,"fecha\0",6)==0){
            if (palabras==1) fecha();
            else printf("Demasiados argumentos\n" );
					}
					else{
						if (strncmp(com,"hora\0",5)==0){
              if (palabras==1) hora();
              else printf("Demasiados argumentos\n" );
						}
						else{
							if (strncmp(com,"hist\0",5)==0){
								 hist(arg,h,palabras);
							}
							else{
								if (((strncmp(com,"fin\0",4)==0) || (strncmp(com,"end\0",4)==0) || (strncmp(com,"exit\0",5)==0)) ){
									if (palabras==1) fin(acabado);
                  else printf("Demasiados argumentos\n" );
								}
								else{
                  if(strncmp(com,"crear\0",6)==0){
                    crear(arg,palabras);
                  }
                  else{
                    if(strncmp(com,"borrar\0",7)==0){
                        borrar(arg,palabras);
                      }
                    else{
                      if(strncmp(com,"listar\0",7)==0){
                          listar(arg);
                      }
                      else{
                        if(strncmp(com,"info\0",5)==0){
                            info(arg,palabras);
                          }
                        else {
                          if(strncmp(com,"asignar\0",8)==0){
                            asignar(arg,palabras,m);
                          }
                          else{
                            if(strncmp(com,"desasignar\0",11)==0){
                              desasignar(arg,palabras,m);
                            }
                            else{
                              if(strncmp(com,"volcar\0",6)==0){
                                volcar(arg,palabras);
                              }
                              else{
                                if(strncmp(com,"llenar\0",7)==0){
                                  llenar(arg,palabras);
                                }
                                else{
                                  if(strncmp(com,"rfich\0",6)==0){
                                    rfich(arg,palabras);
                                  }
                                  else{
                                    if(strncmp(com,"wfich\0",6)==0){
                                      wfich(arg,palabras);
                                    }
                                    else{
                                      if(strncmp(com,"mem\0",4)==0){
                                        mem(arg,palabras,*m);
                                      }
                                      else{
                                        if(strncmp(com,"borrarkey\0",10)==0){
                                          if(palabras==2) borrarkey(arg,palabras);
                                          else printf("Débense introducir só duas palabras\n");
                                        }
                                        else{
                                          if(strncmp(com,"recursiva\0",10)==0){
                                            if(palabras==2)recursiva(atoi(arg));
                                            else printf("Recursiva solo acepta un único número\n");
                                          }
                                          else{
                                            printf("%s no encontrado\n",com );
                                          }
                                        }
                                      }
                                    }
                                  }
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
								}
							}
						}
					}
				}
			}
		}
	}
/*
--------------------------------------------------------------------------------
*/
int main() {
  int acabado = 0;
  int numPalabras;
  char  teclado[MAX];
  char  comando[MAX];
  char argumento[MAX];
	tList historial;
  tListM memoria;
	historial.inicio=NULL;
	historial.final=NULL;
  memoria.inicio=NULL;
  memoria.final=NULL;

  //Mentras non se indica que se acabou leerase por teclado e traballarase coa cadena recivida
	while (acabado != 1){
    printf("->");
    fgets(teclado,MAX,stdin);

    //Gárdase a cadea recivida na lista
		gardar(teclado,&historial);


		//Devolve o comando, o argumento e un número de palabras: 1, 2 ou mais(3)
		numPalabras=TrocearCadena(teclado , comando, argumento);

    //Escóllese a función a facer dependendo do comando
		escollerFuncion(comando,argumento,numPalabras,&acabado,&historial,&memoria);

    //Límpianse as diversas cadeas que se utilizaron
		limpiarBuffer(teclado);
		limpiarBuffer(comando);
		limpiarBuffer(argumento);


	}
  //vacíase a lista antes de pechar o programa
  borrarhist(&historial);
  vaciar(&memoria);
}
