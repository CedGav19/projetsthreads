#include <pthread.h> 
#include <stdio.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h> 
#include <signal.h> 
#include <time.h>
pthread_t threadHandle[5];
int threadHandleActuel; 
 struct sigaction sigAct; 
  void handlerSIGINT(int sig );

pthread_mutex_t mutexparam; 
pthread_mutex_t mutexCompteur ; 
pthread_cond_t condCompteur;
pthread_key_t cle; 

int compteur ;

 typedef struct {
     char nom[20]; 
     int nbSecondes ;
 }DONNEE ;

DONNEE data[] ={
    "MATAGNE",15,
    "WILVERS",10,
    "WAGNER",17,
    "QUETTIER",8,
    "",0
};

DONNEE param ;


void * fctThread(DONNEE *tmp); 
int main() { 

  sigemptyset(&sigAct.sa_mask); 
 sigAct.sa_handler = handlerSIGINT; 
sigaction(SIGINT, &sigAct, NULL); 


 

    printf("debut du programe !! \n");
    int i =0; 
    compteur=0;

    pthread_key_create(&cle, NULL);  

    pthread_mutex_init(&mutexparam, NULL); 
      pthread_mutex_init(&mutexCompteur, NULL); 
    pthread_cond_init(&condCompteur, NULL);
    
    while (strcmp(data[i].nom,"")!=0)
    {
        pthread_mutex_lock(&mutexparam);
        memcpy(&param,&data[i],sizeof(DONNEE));
        printf("thread %d creé   avec le nom  : %s \n ",i,param.nom);
        
         pthread_create(&threadHandle[i], NULL,(void*(*)(void*)) fctThread,&param);

        compteur ++ ;
        i++ ; 
    }


sigset_t mask; 

 /* Le thread  main n'est pas concernés par SIGINT */ 
 sigemptyset(&mask); 
 sigaddset(&mask, SIGINT); 
  sigprocmask(SIG_SETMASK, &mask, NULL); 

    
   // PLUS BESOIN A PARTIR DE ETAPE 3 
    /*while(i!=0)
    {
        pthread_join(threadHandle[i-1], (void **)NULL); 
        printf("pthread %d  fini \n",i);
        i-- ;
    }*/     


//dmd au prof si on peut lancer le master puis masque , si ca masque dans les threqds slave 
 pthread_mutex_lock(&mutexCompteur); 
    while(compteur)
    {
      printf(" il reste %d thread actifs ---------\n",compteur);
        pthread_cond_wait(&condCompteur,&mutexCompteur);
    }

    printf("il ne reste plus aucun thread actif , tout se termine ! ");
 pthread_mutex_unlock(&mutexCompteur); 
   return 0 ; 
 } 


    void * fctThread (DONNEE * tmp) { 
    printf(" DEBUT THREAD %u.%u \n",getpid(),pthread_self());

     char* nom =(char *) malloc( sizeof(length(tmp->nom+1)) );
     strcpy (nom ,tmp->nom );

    char tmpnom[20] ;
    strcpy(tmpnom,tmp->nom);
    int tmpint=tmp->nbSecondes;

    struct timespec t,t2;
   t.tv_sec = tmp->nbSecondes;
   t.tv_nsec = 0;

    t2.tv_sec = 0;
   t2.tv_nsec = 0;

    pthread_mutex_unlock(&mutexparam);

    pthread_setspecific(cle, (void *)nom) ;

    nanosleep(&t,&t2);

    if(t2.tv_sec!=0) {
      nanosleep(&t2,&t2) ;
    }

  printf("texte = %s   \n",(char*) pthread_getspecific(cle));
  printf(" FIN THREAD %u.%u \n",getpid(),pthread_self());


  pthread_mutex_lock(&mutexCompteur);    
  compteur--; 
  pthread_mutex_unlock(&mutexCompteur);
   pthread_cond_signal(&condCompteur); 
  
    pthread_exit(NULL);
 }

  void handlerSIGINT(int sig )
  {

  printf(" je suis le  THREAD %u.%u ",getpid(),pthread_self());
printf("et je m'occupe de  < %s  >\n",(char*) pthread_getspecific(cle));

  }



 // char* nom = malloc(...)
 //strcpy (com , ...)
 // pthread_set_specific(cle , nom );

