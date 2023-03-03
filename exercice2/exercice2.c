#include <pthread.h> 
#include <stdio.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h> 
#include <signal.h> 

pthread_t threadHandle1 ,threadHandle2,threadHandle3,threadHandle4 ,threadHandle; 
int threadHandleActuel; 

void * fctThreadSlave(int p); 
void fctfinMaster(void *p) ;
void * fctThreadMaster(int p); 
 void handlerSIGINT(int sig );
void handlerSIGUSR1(int sig );
 struct sigaction sigAct; 
int main() { 


  sigemptyset(&sigAct.sa_mask);
  sigAct.sa_flags = 0; 
  sigAct.sa_handler = handlerSIGUSR1; 
  sigaction(SIGUSR1, &sigAct, NULL); 
 
 sigAct.sa_handler = handlerSIGINT; 
 sigaction(SIGINT, &sigAct, NULL); 



	int *retThread1, ret ,*retThread2 ,*retThread3 ,*retThread4 ,*retThread ;

 ret = pthread_create(&threadHandle, NULL,(void*(*)(void*)) fctThreadMaster,0);
 ret = pthread_create(&threadHandle1, NULL,(void*(*)(void*)) fctThreadSlave,0);
 ret = pthread_create(&threadHandle2, NULL,(void*(*)(void*)) fctThreadSlave ,0);
 ret = pthread_create(&threadHandle3, NULL,(void*(*)(void*)) fctThreadSlave ,0);
 ret = pthread_create(&threadHandle4, NULL,(void*(*)(void*)) fctThreadSlave ,0);

 sigset_t mask; 

 /* Le thread  main n'est pas concernés par SIGINT */ 
 sigemptyset(&mask); 
 sigaddset(&mask, SIGINT); 
  sigaddset(&mask, SIGUSR1);
 sigprocmask(SIG_SETMASK, &mask, NULL); 
//dmd au prof si on peut lancer le master puis masque , si ca masque dans les threqds slave 

   puts("Thread secondaires lance !"); 
  puts("Attente de la fin des thread secondaire"); 
  ret = pthread_join(threadHandle1, (void **)&retThread1); 
    ret = pthread_join(threadHandle2, (void **)&retThread2); 
    ret = pthread_join(threadHandle3, (void **)&retThread3); 
    ret = pthread_join(threadHandle4, (void **)&retThread4); 
  puts("Fin des threads slave");

    pthread_cancel(threadHandle);
    ret = pthread_join(threadHandle, (void **)&retThread); 
    puts("Fin du thread principal\n");
    exit(1);
 } 


 void * fctThreadSlave (int p) { 

   //struct sigaction sigAct; 
 sigset_t mask; 
 //pthread_cleanup_push(fctThreadClientFin,0);  mets la fonction de terminaison a jour 
 /* Les threads slaves ne sont pas concernés par SIGINT */ 
 sigemptyset(&mask); 
 sigaddset(&mask, SIGINT); 
 
 sigprocmask(SIG_SETMASK, &mask, NULL); 

//  puts("creation d'une fille \n");
  threadHandleActuel=pthread_self();
  printf("je suis le thread slave ( %u ) et j'attend un signal \n",pthread_self());
  pause();

 }
 void * fctThreadMaster (int p) { 

  
     pthread_cleanup_push(fctfinMaster,0);
  int ancEtat;

  if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &ancEtat)) 
    puts("Erreur de setcancelstate");
      if (pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &ancEtat))
       puts("Erreur de setcanceltype");  

//pthread_testcancel();

 //struct sigaction sigAct; 
 sigset_t mask; 
 sigemptyset(&mask); 
 sigaddset(&mask, SIGUSR1); 
 
 sigprocmask(SIG_SETMASK, &mask, NULL); 


  threadHandleActuel=pthread_self();
  printf("je suis le thread Master ( %u ) et j'attend un signal \n",pthread_self());
  while(1) pause();

  pthread_cleanup_pop(1);
 }


 void fctfinMaster(void *p) 
{
  printf("Thread Master terminé!\n");

}


 void handlerSIGINT(int sig )
 {
  /*printf("(%u) j'ai recu un signal SIGINT \n",pthread_self());
    pthread_exit(1);*/
  kill(getpid(),SIGUSR1);

 }


  void handlerSIGUSR1(int sig )
 {
  printf("(%u) j'ai recu un signal SIGUSR1\n",pthread_self());
    pthread_exit(NULL);

 }