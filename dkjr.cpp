#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <SDL/SDL.h>
#include "./presentation/presentation.h"

#define VIDE        		0
#define DKJR       		1
#define CROCO       		2
#define CORBEAU     		3
#define CLE 			4

#define AUCUN_EVENEMENT    	0

#define LIBRE_BAS		1
#define LIANE_BAS		2
#define DOUBLE_LIANE_BAS	3
#define LIBRE_HAUT		4
#define LIANE_HAUT		5

void* FctThreadEvenements(void *);
void* FctThreadCle(void *);
void* FctThreadDK(void *);
void* FctThreadDKJr(void *);
void* FctThreadScore(void *);
void* FctThreadEnnemis(void *);
void* FctThreadCorbeau(void *);
void* FctThreadCroco(void *);

void resetDepart();
void initGrilleJeu();
void setGrilleJeu(int l, int c, int type = VIDE, pthread_t tid = 0);
void afficherGrilleJeu();

void HandlerSIGUSR1(int);
void HandlerSIGUSR2(int);
void HandlerSIGALRM(int);
void HandlerSIGINT(int);
void HandlerSIGQUIT(int);
void HandlerSIGCHLD(int);
void HandlerSIGHUP(int);

void DestructeurVS(void *p);

pthread_t threadCle;
pthread_t threadDK;
pthread_t threadDKJr;
pthread_t threadEvenements;
pthread_t threadScore;
pthread_t threadEnnemis;

pthread_cond_t condDK;
pthread_cond_t condScore;

pthread_mutex_t mutexGrilleJeu;
pthread_mutex_t mutexDK;
pthread_mutex_t mutexEvenement;
pthread_mutex_t mutexScore;

pthread_key_t keySpec;

bool MAJDK = false;
int  score = 0;
bool MAJScore = true;
int  delaiEnnemis = 4000;
int  positionDKJr = 1;
int  evenement = AUCUN_EVENEMENT;
int etatDKJr;

typedef struct
{
  int type;
  pthread_t tid;
} S_CASE;

S_CASE grilleJeu[4][8];

typedef struct
{
  bool haut;
  int position;
} S_CROCO;

// ---------------------------------------------------------RAJOUTE PAR MOI 
struct sigaction sigAct; 



int main(int argc, char* argv[])
{
	int compteurDeMort = 0 ;

	sigemptyset(&sigAct.sa_mask); 
 	sigAct.sa_handler = HandlerSIGQUIT; 
	sigAct.sa_flags = 0; 
	sigaction(SIGQUIT, &sigAct, NULL); 

	sigAct.sa_handler = HandlerSIGALRM; 
	sigAct.sa_flags = 0; 
	sigaction(SIGALRM, &sigAct, NULL);

	sigAct.sa_handler = HandlerSIGINT; 
	sigAct.sa_flags = 0; 
	sigaction(SIGINT, &sigAct, NULL);

	sigAct.sa_handler = HandlerSIGUSR1; 
	sigAct.sa_flags = 0; 
	sigaction(SIGUSR1, &sigAct, NULL);

	sigAct.sa_handler = HandlerSIGHUP; 
	sigAct.sa_flags = 0; 
	sigaction(SIGHUP, &sigAct, NULL);
	sigAct.sa_handler = HandlerSIGCHLD; 
	sigAct.sa_flags = 0; 
	sigaction(SIGCHLD, &sigAct, NULL);
	sigAct.sa_handler = HandlerSIGUSR2; 
	sigAct.sa_flags = 0; 
	sigaction(SIGUSR2, &sigAct, NULL);

    pthread_mutex_init(&mutexEvenement, NULL); 
	pthread_mutex_init(&mutexDK, NULL); 
    pthread_cond_init(&condDK, NULL);
	pthread_key_create(&keySpec, NULL);

	int evt;
	ouvrirFenetreGraphique();

	// lancement des threads 
	pthread_create(&threadCle, NULL,(void*(*)(void*)) FctThreadCle,NULL);
	pthread_create(&threadEvenements, NULL,(void*(*)(void*)) FctThreadEvenements,NULL);
	pthread_create(&threadDKJr, NULL,(void*(*)(void*)) FctThreadDKJr,NULL);
	pthread_create(&threadDK, NULL,(void*(*)(void*)) FctThreadDK,NULL);
	pthread_create(&threadEnnemis, NULL,(void*(*)(void*)) FctThreadEnnemis,NULL);

	//masquage des signaux 
	sigset_t mask; 
	sigemptyset(&mask); 
	sigaddset(&mask, SIGINT); 
	sigaddset(&mask, SIGALRM); 
	sigaddset(&mask, SIGQUIT); 
	sigaddset(&mask, SIGUSR1); 
	sigaddset(&mask, SIGHUP); 
	sigaddset(&mask, SIGCHLD); 
	sigaddset(&mask, SIGUSR2);  
   	sigprocmask(SIG_SETMASK, &mask, NULL); 

	/*afficherCage(2);
	afficherCage(3);
	afficherCage(4);
	afficherRireDK();
	afficherCle(3);
	afficherCroco(11, 2);
	afficherCorbeau(10, 2);*/
	
	effacerCarres(0, 0, 7, 3);
	afficherScore(score);

	while (compteurDeMort<2)
	{
		
		pthread_join(threadDKJr, (void **)NULL);
		printf("DKJR est dead \n");
		compteurDeMort++;
		afficherEchec(compteurDeMort);
		pthread_create(&threadDKJr, NULL,(void*(*)(void*)) FctThreadDKJr,NULL);
		effacerCarres(11,7, 2, 2);
		resetDepart();
	}
		pthread_join(threadDKJr, (void **)NULL);
		compteurDeMort++;
		afficherEchec(compteurDeMort);
		pthread_mutex_lock(&mutexGrilleJeu);
		while(1);
		//pthread_join(threadEvenements, (void **)NULL);
		pthread_join(threadEvenements, NULL);


}

// -------------------------------------

void initGrilleJeu()
{
  int i, j;   
  
  pthread_mutex_lock(&mutexGrilleJeu);

  for(i = 0; i < 4; i++)
    for(j = 0; j < 7; j++)
      setGrilleJeu(i, j);

  pthread_mutex_unlock(&mutexGrilleJeu);
}

// -------------------------------------

void setGrilleJeu(int l, int c, int type, pthread_t tid)
{
  grilleJeu[l][c].type = type;
  grilleJeu[l][c].tid = tid;
}

// -------------------------------------

void afficherGrilleJeu()
{   
   for(int j = 0; j < 4; j++)
   {
       for(int k = 0; k < 8; k++)
          printf("%d  ", grilleJeu[j][k].type);
       printf("\n");
   }

   printf("\n");   
}


/////***********************************************************\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\//

void* FctThreadCle(void *)
{
	//masquage des signaux
	sigset_t mask; 
	sigemptyset(&mask); 
	sigaddset(&mask, SIGINT); 
	sigaddset(&mask, SIGALRM); 
	sigaddset(&mask, SIGQUIT); 
	sigaddset(&mask, SIGUSR1); 
	sigaddset(&mask, SIGHUP); 
	sigaddset(&mask, SIGCHLD); 
	sigaddset(&mask, SIGUSR2); 
   	sigprocmask(SIG_SETMASK, &mask, NULL); 

	int i = 1 ;
	int contexte =1 ; // 1 = aller , 0 = retour 
	 struct timespec t;
 	  t.tv_sec = 0;
	   t.tv_nsec = 700000000;
	while(1)
	{
		pthread_mutex_lock(&mutexGrilleJeu);
		afficherCle(i);
		pthread_mutex_unlock(&mutexGrilleJeu);
	
		switch(i)
		{
		
			case 1 : 
					grilleJeu[0][1].type=CLE ;
				 	nanosleep(&t,NULL);
					i++ ;
					contexte=1 ;//aller 
			break ;
			case 2 : 
			case 3 :
					grilleJeu[0][1].type=VIDE ;
				 	nanosleep(&t,NULL);
					if(contexte==1)i++;
					else i-- ;
			break ;
			case 4 :
					grilleJeu[0][1].type=VIDE ;
				 	nanosleep(&t,NULL);
					i-- ;
					contexte=0 ; //retour 
			break ;

		}
		pthread_mutex_lock(&mutexGrilleJeu);
		effacerCarres(3,12,2,3);
		pthread_mutex_unlock(&mutexGrilleJeu);
	}

}

void * FctThreadEvenements (void *)
{

	//masquage des signaux
	sigset_t mask; 
	sigemptyset(&mask); 
	sigaddset(&mask, SIGINT); 
	sigaddset(&mask, SIGALRM); 
	sigaddset(&mask, SIGQUIT); 
	sigaddset(&mask, SIGUSR1); 
	sigaddset(&mask, SIGHUP); 
	sigaddset(&mask, SIGCHLD); 
	sigaddset(&mask, SIGUSR2); 
   	sigprocmask(SIG_SETMASK, &mask, NULL); 
	 

	while (1)
	{
		pthread_mutex_lock(&mutexEvenement);
		evenement= AUCUN_EVENEMENT;
	
	    evenement = lireEvenement();
		
		pthread_mutex_unlock(&mutexEvenement);
		
	    switch (evenement)
	    {
		case SDL_QUIT:
			exit(0);
		case SDLK_UP:
			printf("KEY_UP\n");
			break;
		case SDLK_DOWN:
			printf("KEY_DOWN\n");
			break;
		case SDLK_LEFT:
			printf("KEY_LEFT\n");
			break;
		case SDLK_RIGHT:
			printf("KEY_RIGHT\n");
			break ;
		case AUCUN_EVENEMENT :
			printf("AUCUN_EVENEMENT\n");
			break ;
	    }
		pthread_kill(threadDKJr,SIGQUIT);           
		struct timespec t;
 	    t.tv_sec = 0;
  		t.tv_nsec = 100000000;
		nanosleep(&t,NULL);
	}
	
}

void* FctThreadDKJr(void* p)
{
	sigset_t mask; 
	sigemptyset(&mask);
	sigaddset(&mask, SIGALRM); 
	sigaddset(&mask, SIGUSR1); 
	sigaddset(&mask, SIGUSR2); 
   	sigprocmask(SIG_SETMASK, &mask, NULL); 

	struct timespec tsaut;
 	tsaut.tv_sec = 1;
  	tsaut.tv_nsec = 400000000;
	struct timespec t;
 	t.tv_sec = 0;
  	t.tv_nsec = 700000000;
	bool on = true; 
	pthread_mutex_lock(&mutexGrilleJeu);
	
	setGrilleJeu(3, 1, DKJR); 
	afficherDKJr(11, 9, 1); 
	etatDKJr = LIBRE_BAS; 
	positionDKJr = 1;
	
	pthread_mutex_unlock(&mutexGrilleJeu);
	while (on)
	{
		pause();
		pthread_mutex_lock(&mutexEvenement);
		pthread_mutex_lock(&mutexGrilleJeu);
		switch (etatDKJr)
		{
			case LIBRE_BAS:
							switch (evenement)
							{
								case SDLK_LEFT:
												if (positionDKJr > 1)
												{
													if(grilleJeu[3][positionDKJr-1].type == CROCO)
													{
														setGrilleJeu(2, positionDKJr);
														
														printf("LIBRE BAS ,  va sur le croco a gauche\n");
														setGrilleJeu(3, positionDKJr);
														effacerCarres(11, (positionDKJr * 2) + 7, 2, 2);
														pthread_kill(grilleJeu[3][positionDKJr-1].tid ,SIGUSR2);  
														pthread_mutex_unlock(&mutexGrilleJeu);
														pthread_mutex_unlock(&mutexEvenement);
														pthread_exit(0);
													}
													else 
													{
													//on efface l'ancien dkjr
													setGrilleJeu(3, positionDKJr); // met le type a vide , donc rien dedans  
													effacerCarres(11, (positionDKJr * 2) + 7, 2, 2);
													//modif la valeur
													positionDKJr--;
													//afiiche le nv 
													setGrilleJeu(3, positionDKJr, DKJR);
													afficherDKJr(11, (positionDKJr * 2) + 7, 
													((positionDKJr - 1) % 4) + 1);
													}
												}
												else 
												{
													if(positionDKJr==1)
													{
														printf("libre bas , tombe buisson \n");
														setGrilleJeu(3, positionDKJr);
														effacerCarres(11, (positionDKJr * 2) + 7, 2, 2);
														positionDKJr--;
														
														afficherDKJr(11, (positionDKJr * 2) + 7, 
														13);
														pthread_mutex_unlock(&mutexGrilleJeu);
														pthread_mutex_unlock(&mutexEvenement);
														nanosleep(&t,NULL);
														pthread_exit(0);
													}
												}
								break;
								case SDLK_RIGHT:
												if (positionDKJr < 7)
												{
													if(grilleJeu[3][positionDKJr+1].type == CROCO)
													{
														setGrilleJeu(2, positionDKJr);
														
														printf("LIBRE BAS ,  va sur le croco a droite\n");
														setGrilleJeu(3, positionDKJr);
														effacerCarres(11, (positionDKJr * 2) + 7, 2, 2);
														pthread_kill(grilleJeu[3][positionDKJr+1].tid ,SIGUSR2);  
														pthread_mutex_unlock(&mutexGrilleJeu);
														pthread_mutex_unlock(&mutexEvenement);
														pthread_exit(0);
													}
													else 
													{
													setGrilleJeu(3, positionDKJr);
													effacerCarres(11, (positionDKJr * 2) + 7, 2, 2);
													positionDKJr++;
													setGrilleJeu(3, positionDKJr, DKJR);
													afficherDKJr(11, (positionDKJr * 2) + 7, 
													((positionDKJr - 1) % 4) + 1);
													}
												}											
								break;
								case SDLK_UP:

										if(grilleJeu[2][positionDKJr].type == CORBEAU)
										{
											pthread_kill(grilleJeu[2][positionDKJr].tid ,SIGUSR1);   
											setGrilleJeu(3, positionDKJr);
											effacerCarres(11, (positionDKJr * 2) + 7, 2, 2);
											pthread_mutex_unlock(&mutexGrilleJeu);
											pthread_mutex_unlock(&mutexEvenement);
											pthread_exit(0);
										
										}
										else 
										{
											
											if(positionDKJr==1||positionDKJr==5)
											{
												setGrilleJeu(3, positionDKJr);
												effacerCarres(11, (positionDKJr * 2) + 7, 2, 2);

												setGrilleJeu(2, positionDKJr, DKJR);
												afficherDKJr(10, (positionDKJr * 2) + 7, 
												7);
												etatDKJr=LIANE_BAS;
											}
											else 
											{
												if(positionDKJr==7)
												{
													setGrilleJeu(3, positionDKJr);
													effacerCarres(11, (positionDKJr * 2) + 7, 2, 2);

													setGrilleJeu(2, positionDKJr, DKJR);
													afficherDKJr(10, (positionDKJr * 2) + 7, 
													7);
													etatDKJr=DOUBLE_LIANE_BAS;
												}
												else 
												{
													setGrilleJeu(3, positionDKJr);
													effacerCarres(11, (positionDKJr * 2) + 7, 2, 2);

													setGrilleJeu(2, positionDKJr, DKJR);
													afficherDKJr(10, (positionDKJr * 2) + 7, 
													8);
													afficherGrilleJeu();
													
													pthread_mutex_unlock(&mutexGrilleJeu);
													nanosleep(&tsaut ,NULL);
													pthread_mutex_lock(&mutexGrilleJeu);
													afficherGrilleJeu();
													if(grilleJeu[3][positionDKJr].type == CROCO)
													{
														setGrilleJeu(2, positionDKJr);
														
														printf("tombe sur croco ( en bas ) \n");
														setGrilleJeu(2, positionDKJr);
														effacerCarres(10, (positionDKJr * 2) + 7, 2, 2);
														pthread_kill(grilleJeu[3][positionDKJr].tid ,SIGUSR2);  
														pthread_mutex_unlock(&mutexGrilleJeu);
														pthread_mutex_unlock(&mutexEvenement);
														pthread_exit(0);
													}
													else 
													{
														setGrilleJeu(2, positionDKJr);
														effacerCarres(10, (positionDKJr * 2) + 7, 2, 2);

														setGrilleJeu(3, positionDKJr, DKJR);
														afficherDKJr(11, (positionDKJr * 2) + 7, 
														((positionDKJr - 1) % 4) + 1);
													}
													
													
												}
											}
										}	
								break;
							}
			case LIANE_BAS:
							if(evenement==SDLK_DOWN)
							{
								if(grilleJeu[3][positionDKJr].type == CROCO)
								{
									setGrilleJeu(2, positionDKJr);
									
									printf("tombe sur croco a partir d'une liane( en bas ) \n");
									setGrilleJeu(2, positionDKJr);
									effacerCarres(10, (positionDKJr * 2) + 7, 2, 2);
									pthread_kill(grilleJeu[3][positionDKJr].tid ,SIGUSR2);  
									pthread_mutex_unlock(&mutexGrilleJeu);
									pthread_mutex_unlock(&mutexEvenement);
									pthread_exit(0);
								}
								else 
								{

									setGrilleJeu(2, positionDKJr);
									effacerCarres(10, (positionDKJr * 2) + 7, 2, 2);

									setGrilleJeu(3, positionDKJr, DKJR);
									afficherDKJr(11, (positionDKJr * 2) + 7, 
									((positionDKJr - 1) % 4) + 1);	
									etatDKJr=LIBRE_BAS;
								}
							}
			break;
			case DOUBLE_LIANE_BAS:
								if(evenement==SDLK_DOWN)
								{
									if(grilleJeu[3][positionDKJr].type == CROCO)
									{
										setGrilleJeu(2, positionDKJr);
										
										printf("tombe sur croco a partir d'une liane( en bas ) \n");
										setGrilleJeu(2, positionDKJr);
										effacerCarres(10, (positionDKJr * 2) + 7, 2, 2);
										pthread_kill(grilleJeu[3][positionDKJr].tid ,SIGUSR2);  
										pthread_mutex_unlock(&mutexGrilleJeu);
										pthread_mutex_unlock(&mutexEvenement);
										pthread_exit(0);
									}
									else 
									{
										setGrilleJeu(2, positionDKJr);
										effacerCarres(10, (positionDKJr * 2) + 7, 2, 2);

										setGrilleJeu(3, positionDKJr, DKJR);
										afficherDKJr(11, (positionDKJr * 2) + 7, 
										((positionDKJr - 1) % 4) + 1);	
										etatDKJr=LIBRE_BAS;
									}
								}
								else 
								{
									if(evenement==SDLK_UP)
									{
										setGrilleJeu(2, positionDKJr);
										effacerCarres(10, (positionDKJr * 2) + 7, 2, 2);

										setGrilleJeu(1, positionDKJr, DKJR);
										afficherDKJr(7, (positionDKJr * 2) + 7, 
										6);	
										etatDKJr=LIBRE_HAUT;										
									}
								}

			break;
			case LIBRE_HAUT:
						switch(evenement)
						{
							case SDLK_DOWN :
											if(positionDKJr==7)
											{
												if(grilleJeu[2][positionDKJr].type == CORBEAU)
												{
													pthread_kill(grilleJeu[2][positionDKJr].tid ,SIGUSR1);   
													setGrilleJeu(1, positionDKJr);
													effacerCarres(7, (positionDKJr * 2) + 7, 2, 2);
													pthread_mutex_unlock(&mutexGrilleJeu);
													pthread_mutex_unlock(&mutexEvenement);
													pthread_exit(0);
												
												}
												else 
												{
													setGrilleJeu(1, positionDKJr);
													effacerCarres(7, (positionDKJr * 2) + 7, 2, 2);

													setGrilleJeu(2, positionDKJr, DKJR);
													afficherDKJr(10, (positionDKJr * 2) + 7, 
													7);
													etatDKJr=DOUBLE_LIANE_BAS;
												}
											}
							break;
							case SDLK_RIGHT:
											if (positionDKJr < 6)
											{
												if(grilleJeu[1][positionDKJr+1].type == CROCO)
													{
														setGrilleJeu(2, positionDKJr);
														
														printf("LIBRE haut ,  va sur le croco a droite\n");
														setGrilleJeu(1, positionDKJr);
														effacerCarres(7, (positionDKJr * 2) + 7, 2, 2);
														pthread_kill(grilleJeu[1][positionDKJr+1].tid ,SIGUSR2);  
														pthread_mutex_unlock(&mutexGrilleJeu);
														pthread_mutex_unlock(&mutexEvenement);
														pthread_exit(0);
													}
													else 
													{
														setGrilleJeu(1, positionDKJr);
														effacerCarres(7, (positionDKJr * 2) + 7, 2, 2);
														positionDKJr++;
														setGrilleJeu(1, positionDKJr, DKJR);
														afficherDKJr(7, (positionDKJr * 2) + 7, 
														((positionDKJr - 1) % 4) + 1);
													}
											}
											else 
											{
												if(positionDKJr==6)
												{
													setGrilleJeu(1, positionDKJr);
													effacerCarres(7, (positionDKJr * 2) + 7, 2, 2);
													positionDKJr++;
													setGrilleJeu(1, positionDKJr, DKJR);
													afficherDKJr(7, (positionDKJr * 2) + 7, 
													6);
													
												}
											}											
							break;
							case SDLK_LEFT:
											if (positionDKJr > 3&& positionDKJr<=7)
											{
												if(grilleJeu[1][positionDKJr-1].type == CROCO)
													{
														setGrilleJeu(2, positionDKJr);
														
														printf("LIBRE haut ,  va sur le croco a gauche\n");
														setGrilleJeu(1, positionDKJr);
														effacerCarres(7, (positionDKJr * 2) + 7, 2, 2);
														pthread_kill(grilleJeu[1][positionDKJr-1].tid ,SIGUSR2);  
														pthread_mutex_unlock(&mutexGrilleJeu);
														pthread_mutex_unlock(&mutexEvenement);
														pthread_exit(0);
													}
													else 
													{
														setGrilleJeu(1, positionDKJr);
														effacerCarres(7, (positionDKJr * 2) + 7, 2, 2);
														positionDKJr--;
														setGrilleJeu(1, positionDKJr, DKJR);
														afficherDKJr(7, (positionDKJr * 2) + 7, 
														((positionDKJr - 1) % 4) + 1);
													}
											}
											else
											{
												if (positionDKJr==7)
												{
													if(grilleJeu[1][positionDKJr-1].type == CROCO)
													{
														setGrilleJeu(2, positionDKJr);
														
														printf("LIBRE BAS ,  va sur le croco a gauche\n");
														setGrilleJeu(1, positionDKJr);
														effacerCarres(7, (positionDKJr * 2) + 7, 2, 2);
														pthread_kill(grilleJeu[1][positionDKJr-1].tid ,SIGUSR2);  
														pthread_mutex_unlock(&mutexGrilleJeu);
														pthread_mutex_unlock(&mutexEvenement);
														pthread_exit(0);
													}
													else 
													{
													setGrilleJeu(1, positionDKJr);
													effacerCarres(8, (positionDKJr * 2) + 7, 2, 2);
													positionDKJr--;
													setGrilleJeu(1, positionDKJr, DKJR);
													afficherDKJr(7, (positionDKJr * 2) + 7, 
													((positionDKJr - 1) % 4) + 1);
													}
												}
												if (positionDKJr==3)
												{
													setGrilleJeu(1, positionDKJr);
													effacerCarres(7, (positionDKJr * 2) + 7, 2, 2);
													if(grilleJeu[0][1].type!=CLE )
													{
														//animation echec 
														printf("TOMBE DS LE BUISSON DE HAUT \n ");
														afficherDKJr(5, 12, 9);
														nanosleep(&tsaut,NULL);
														effacerCarres(5, 12, 3, 2);

														afficherDKJr(9, 9, 12);
														nanosleep(&tsaut,NULL);
														effacerCarres(6, 11, 2, 2);

														afficherDKJr(11, 7, 13);
														pthread_mutex_unlock(&mutexGrilleJeu);
														pthread_mutex_unlock(&mutexEvenement);
														nanosleep(&t,NULL);
														pthread_exit(0);
													}
													else 
													{
														//réussite
														afficherDKJr(5, 12, 9);
														nanosleep(&tsaut,NULL);
														effacerCarres(5, 12, 3, 3);

														afficherDKJr(3, 11, 10);
														nanosleep(&tsaut,NULL);
														effacerCarres(3, 11, 3, 3);

														afficherDKJr(6, 10, 11);
														nanosleep(&tsaut,NULL);
														effacerCarres(6,10,2,3);

														resetDepart();
														positionDKJr=1;
														etatDKJr=LIBRE_BAS;
														setGrilleJeu(3, 1, DKJR); 
														afficherDKJr(11, 9, 1); 

														pthread_mutex_lock(&mutexScore);
														score+=10;
														MAJScore =true;
														pthread_mutex_unlock(&mutexScore);
														pthread_create(&threadScore, NULL,(void*(*)(void*)) FctThreadScore,NULL);
														pthread_cond_signal(&condDK); 
														
														
														
													}
												}
											}
											
							break;
							case SDLK_UP:
										if(positionDKJr==6)
										{
												setGrilleJeu(1, positionDKJr);
												effacerCarres(7, (positionDKJr * 2) + 7, 2, 2);
												setGrilleJeu(0, positionDKJr, DKJR);
												afficherDKJr(6, (positionDKJr * 2) + 7, 
												7);
												etatDKJr=LIANE_HAUT;
										}
										else 
										{
											if(positionDKJr!=7)
											{
												setGrilleJeu(1, positionDKJr);
												effacerCarres(7, (positionDKJr * 2) + 7, 2, 2);

												setGrilleJeu(0, positionDKJr, DKJR);
												afficherDKJr(6, (positionDKJr * 2) + 7, 
												8);
		
													pthread_mutex_unlock(&mutexGrilleJeu);
													nanosleep(&tsaut ,NULL);
													pthread_mutex_lock(&mutexGrilleJeu);
													afficherGrilleJeu();
													if(grilleJeu[1][positionDKJr].type == CROCO)
													{
														
														printf("tombe sur croco ( en haut ) \n");
														setGrilleJeu(0, positionDKJr);
														effacerCarres(6, (positionDKJr * 2) + 7, 2, 2);
														pthread_kill(grilleJeu[1][positionDKJr].tid ,SIGUSR2);  
														pthread_mutex_unlock(&mutexGrilleJeu);
														pthread_mutex_unlock(&mutexEvenement);
														pthread_exit(0);
													}
													else 
													{
														setGrilleJeu(0, positionDKJr);
														effacerCarres(6, (positionDKJr * 2) + 7, 2, 2);

														setGrilleJeu(1, positionDKJr, DKJR);
														afficherDKJr(7, (positionDKJr * 2) + 7, 
														((positionDKJr - 1) % 4) + 1);
													}
													
													
											}
										}


							break;
						}
			break;
		case LIANE_HAUT:
						if(grilleJeu[1][positionDKJr].type == CROCO)
						{
							printf("tombe sur croco ( en haut  avec la liane ) \n");
							setGrilleJeu(0, positionDKJr);
							effacerCarres(6, (positionDKJr * 2) + 7, 2, 2);
							pthread_kill(grilleJeu[1][positionDKJr].tid ,SIGUSR2);  
							pthread_mutex_unlock(&mutexGrilleJeu);
							pthread_mutex_unlock(&mutexEvenement);
							pthread_exit(0);
						}
						else 
						{
							setGrilleJeu(0, positionDKJr);
							effacerCarres(6, (positionDKJr * 2) + 7, 2, 2);
							setGrilleJeu(1, positionDKJr, DKJR);
							afficherDKJr(7, (positionDKJr * 2) + 7, 
							((positionDKJr - 1) % 4) + 1);
							etatDKJr=LIBRE_HAUT;
						}
		break;
		}
		pthread_mutex_unlock(&mutexGrilleJeu);
		pthread_mutex_unlock(&mutexEvenement);
	}
	pthread_exit(0);	
}

void* FctThreadDK(void* p)
{
	sigset_t mask; 
	sigemptyset(&mask); 
	sigaddset(&mask, SIGINT); 
	sigaddset(&mask, SIGALRM); 
	sigaddset(&mask, SIGQUIT); 
	sigaddset(&mask, SIGUSR1); 
	sigaddset(&mask, SIGHUP); 
	sigaddset(&mask, SIGCHLD); 
	sigaddset(&mask, SIGUSR2);  
   	sigprocmask(SIG_SETMASK, &mask, NULL); 

	pthread_mutex_lock(&mutexDK); 
		struct timespec t;
	t.tv_sec = 0;
  	t.tv_nsec = 700000000;
	  for (int i =1 ;i<=4;i++) afficherCage(i);
	  int cleAttrapee = 0;
	
	  while(1)
    {
		
        pthread_cond_wait(&condDK,&mutexDK);
		printf("dkjr a attrapé la cle\n");
		cleAttrapee++;
		if(cleAttrapee%4==0)
		{
			effacerCarres(2, 7, 5,5);
			afficherRireDK();
			nanosleep(&t,NULL);
			//modif du score 
			pthread_mutex_lock(&mutexScore);
			score+=10;
			MAJScore =true;
			pthread_mutex_unlock(&mutexScore);
			pthread_create(&threadScore, NULL,(void*(*)(void*)) FctThreadScore,NULL);
		}
		pthread_mutex_lock(&mutexGrilleJeu);
		effacerCarres(2, 7, 5,5);
		pthread_mutex_unlock(&mutexGrilleJeu);

		for (int i =(cleAttrapee%4)+1 ;i<=4;i++) afficherCage(i);
    }
	pthread_mutex_unlock(&mutexDK); 
}

void* FctThreadScore(void* p)
{
	sigset_t mask; 
	sigemptyset(&mask); 
	sigaddset(&mask, SIGINT); 
	sigaddset(&mask, SIGALRM); 
	sigaddset(&mask, SIGQUIT); 
	sigaddset(&mask, SIGUSR1); 
	sigaddset(&mask, SIGHUP); 
	sigaddset(&mask, SIGCHLD); 
	sigaddset(&mask, SIGUSR2); ; 
   	sigprocmask(SIG_SETMASK, &mask, NULL); 

	if(MAJScore==true) 
	{
		 afficherScore(score);
		 MAJScore=false;
	}
	pthread_exit(NULL);
}

void* FctThreadEnnemis(void* p)
{
	sigset_t mask; 
	sigemptyset(&mask); 
 	sigaddset(&mask, SIGINT); 
	sigaddset(&mask, SIGQUIT); 
	sigaddset(&mask, SIGUSR1); 
	sigaddset(&mask, SIGHUP); 
	sigaddset(&mask, SIGCHLD); 
	sigaddset(&mask, SIGUSR2);   
   	sigprocmask(SIG_SETMASK, &mask, NULL); 
	alarm(15);
	int RDMennemi ;
	srand(time(NULL));
	while(1)
	{
		pthread_t threadCorbeau, threadCroco;
		struct timespec t;
		t.tv_sec = delaiEnnemis/1000;
		t.tv_nsec =delaiEnnemis-(((int)delaiEnnemis/1000)*1000);
	
	
		nanosleep(&t,NULL);
		RDMennemi = rand();
		if(RDMennemi%2==0)
		{
			printf("Spawn de croco \n");
			pthread_create(&threadCroco, NULL, (void*(*)(void*))FctThreadCroco, NULL);
		}
		else 
		{
			printf("Spawn de corbeau \n");
			pthread_create(&threadCorbeau, NULL, (void*(*)(void*))FctThreadCorbeau, NULL);
		}
		printf("delaiennemi = %d \n",delaiEnnemis);

	}
}

void *FctThreadCorbeau(void* p)
{

	sigset_t mask; 
	sigemptyset(&mask); 
	sigaddset(&mask, SIGINT); 
	sigaddset(&mask, SIGALRM); 
	sigaddset(&mask, SIGQUIT); 
	sigaddset(&mask, SIGHUP); 
	sigaddset(&mask, SIGCHLD); 
	sigaddset(&mask, SIGUSR2); 
   	sigprocmask(SIG_SETMASK, &mask, NULL); 

	timespec t ; 
	t.tv_sec = 0;
	t.tv_nsec = 700000000;

	int positionCorbeau = 0;
	int *spe =(int *)malloc(1 * sizeof(int));
	*spe = positionCorbeau;
	pthread_setspecific(keySpec, spe);
	setGrilleJeu(2, positionCorbeau, CORBEAU, pthread_self());
	afficherCorbeau((positionCorbeau * 2) + 8, 1);
	
	nanosleep(&t, NULL);
	while(1)
	{
		pthread_mutex_lock(&mutexGrilleJeu);
		if(positionCorbeau < 7)
		{	

			if(grilleJeu[2][positionCorbeau +1].type == DKJR)
			{	
				printf("collision corbeau dk \n");
				effacerCarres(9, (positionCorbeau * 2) + 8, 2, 2);
				setGrilleJeu(2, positionCorbeau);
				pthread_kill(threadDKJr,SIGINT);     
				pthread_mutex_unlock(&mutexGrilleJeu);
				pthread_exit(0);
			}
			else
			{


				if(positionCorbeau%2 == 0)
				{
					effacerCarres(10, (positionCorbeau * 2) + 8, 1, 2);
					setGrilleJeu(2, positionCorbeau);
					positionCorbeau ++;
					*spe = positionCorbeau;
					pthread_setspecific(keySpec, spe);
					setGrilleJeu(2, positionCorbeau, CORBEAU, pthread_self());
					afficherCorbeau((positionCorbeau * 2) + 8, 2);
					
				}
				else 
				{
					effacerCarres(9, (positionCorbeau * 2) + 8, 2, 2);
					setGrilleJeu(2, positionCorbeau);
					positionCorbeau ++;
					*spe = positionCorbeau;
					pthread_setspecific(keySpec, spe);
					setGrilleJeu(2, positionCorbeau, CORBEAU, pthread_self());
				    afficherCorbeau((positionCorbeau * 2) + 8, 1);
				}
				

				
			}
			
		}	
		else
		{	
			printf("fin corbeau  \n");
			pthread_mutex_unlock(&mutexGrilleJeu);
			setGrilleJeu(2, positionCorbeau);
			effacerCarres(9, (positionCorbeau * 2) + 8, 2, 2);
			free(spe);
			pthread_exit(0);
		}
		pthread_mutex_unlock(&mutexGrilleJeu);
		nanosleep(&t, NULL);
	}
	pthread_exit(NULL);

}


void *FctThreadCroco(void* p)
{
	sigset_t mask; 
	sigemptyset(&mask); 
	sigaddset(&mask, SIGINT); 
	sigaddset(&mask, SIGALRM); 
	sigaddset(&mask, SIGQUIT); 
	sigaddset(&mask, SIGUSR1); 
	sigaddset(&mask, SIGHUP); 
	sigaddset(&mask, SIGCHLD); 
   	sigprocmask(SIG_SETMASK, &mask, NULL); 

	timespec t;
	S_CROCO Croco;
	Croco.haut = true;
	Croco.position = 2;
	S_CROCO *spe =(S_CROCO *)malloc(1 * sizeof(S_CROCO));
	t.tv_nsec = 700000000;
	t.tv_sec = 0;

	pthread_mutex_lock(&mutexGrilleJeu);
	setGrilleJeu(1, Croco.position, CROCO, pthread_self());
	afficherCroco((Croco.position * 2) + 7, Croco.position%2+1);
	pthread_mutex_unlock(&mutexGrilleJeu);
	nanosleep(&t, NULL);

	while(1)
	{
		pthread_mutex_lock(&mutexGrilleJeu);
		if(Croco.haut)
		{
		
			if(Croco.position < 7)
			{
				if(grilleJeu[1][Croco.position+1].type == DKJR)
				{
					printf("envoie du signal sighup \n");
					pthread_kill(threadDKJr,SIGHUP);
					effacerCarres(8, (Croco.position * 2) + 7, 1, 2); 
					setGrilleJeu(1, Croco.position);
					pthread_mutex_unlock(&mutexGrilleJeu);
					pthread_exit(0);

				}
				else
				{
					effacerCarres(8, ((Croco.position  )* 2) + 7, 2, 2);
					setGrilleJeu(1, Croco.position );
					Croco.position ++;
					*spe = Croco;
					setGrilleJeu(1, Croco.position, CROCO, pthread_self());
					afficherCroco((Croco.position * 2) + 7, Croco.position%2+1);
					afficherGrilleJeu();
					pthread_setspecific(keySpec, spe);
				}
			}
			else
			{	
				effacerCarres(8, ((Croco.position  )* 2) + 7, 2, 2);
				setGrilleJeu(1, Croco.position );
				Croco.haut = false;

				if (grilleJeu[1][7].type == DKJR ) // double liane 
				{
					printf("envoie du signal sighup double liane  \n");
					pthread_kill(threadDKJr,SIGHUP);
					effacerCarres(8, (Croco.position * 2) + 7, 2, 3); 
					setGrilleJeu(1, Croco.position);
					setGrilleJeu(3, Croco.position);
					pthread_mutex_unlock(&mutexGrilleJeu);
					pthread_exit(0);
				}
				else {
					afficherCroco((Croco.position * 2) + 7, 3);
					*spe = Croco;
				}
			}
		}
		else
		{	
			if(Croco.position > 1 )
			{	
				if( Croco.position!=7 && grilleJeu[3][Croco.position-1].type == DKJR)
				{
					// envoyeer un signal a dkjr 
					printf("envoie du signal sigchld \n");
					pthread_kill(threadDKJr,SIGCHLD);
					setGrilleJeu(3, Croco.position );
					pthread_mutex_unlock(&mutexGrilleJeu);
					pthread_exit(0);

				}
				else
				{
					if(Croco.position == 7)
					{	
						printf("gestion chute croco\n");
						effacerCarres(9, 23);
						
						if(grilleJeu[3][Croco.position].type == DKJR)
						{
							printf("envoie du signal  dk en bas des doubles lianes   \n");
							pthread_kill(threadDKJr,SIGCHLD);
							effacerCarres(12, (Croco.position * 2) + 7, 2, 3); 
						
							setGrilleJeu(3, Croco.position);
							pthread_mutex_unlock(&mutexGrilleJeu);
							pthread_exit(0);
						}
						else 
						{
							setGrilleJeu(3, Croco.position, CROCO, pthread_self());
							*spe = Croco;
							pthread_setspecific(keySpec, spe);
							afficherCroco((Croco.position * 2) + 8, (Croco.position% 2) + 4);
							afficherGrilleJeu();
							pthread_mutex_unlock(&mutexGrilleJeu);
							nanosleep(&t, NULL);
							pthread_mutex_lock(&mutexGrilleJeu);

							//premier deplacement vers la gauche 
							if(grilleJeu[3][Croco.position-1].type == DKJR)
							{
								printf("envoie du signal sigchld \n");
								pthread_kill(threadDKJr,SIGCHLD);
								setGrilleJeu(3, Croco.position );
								pthread_mutex_unlock(&mutexGrilleJeu);
								pthread_exit(0);
							}
							else 
							{
								effacerCarres(12, ((Croco.position  )* 2) + 8, 1, 1);
								setGrilleJeu(3, Croco.position );
								Croco.position --;
								*spe = Croco;
								setGrilleJeu(3, Croco.position, CROCO, pthread_self());
								afficherCroco((Croco.position * 2) + 8, (Croco.position % 2) + 4);
								afficherGrilleJeu();
								
								pthread_setspecific(keySpec, spe);
							}
						}
						
					}
					else
					{
						effacerCarres(12, ((Croco.position  )* 2) + 8, 1, 1);
						setGrilleJeu(3, Croco.position );
						Croco.position --;
						*spe = Croco;
						setGrilleJeu(3, Croco.position, CROCO, pthread_self());
						afficherCroco((Croco.position * 2) + 8, (Croco.position % 2) + 4);
						afficherGrilleJeu();
						pthread_setspecific(keySpec, spe);
					}
				}
			}
			else
			{
				effacerCarres(12, ((Croco.position )* 2) + 8, 1, 1);
				setGrilleJeu(3, Croco.position);
				pthread_mutex_unlock(&mutexGrilleJeu);
				pthread_exit(0);
			}

			
		}
		pthread_mutex_unlock(&mutexGrilleJeu);
		nanosleep(&t, NULL);
		
	}
	pthread_exit(0);
}

void resetDepart()
{
	printf("dans resetdepart\n");
	effacerCarres( 9,7, 2, 6);
	effacerCarres( 11,9, 2, 6);
	for(int i = 0 ; i<3 ; i++)
	{
		if(grilleJeu[2][i].type == CORBEAU )
		{
			printf("enleve 1 corbeau\n");
			pthread_kill(grilleJeu[2][i].tid ,SIGUSR1); 
		}
		
	}
	for (int i = 1 ; i<4;i++)
	{
		if(grilleJeu[3][i].type == CROCO )
		{
			printf("enleve 1 croco\n");
			pthread_kill(grilleJeu[3][i].tid ,SIGUSR2); 
		}
	}
}

void HandlerSIGQUIT(int sig )
{
	//printf("dkjr recoit sigquit\n");
}
void HandlerSIGALRM(int s )
{
	delaiEnnemis-=250;
	if(delaiEnnemis > 2500)
	{
		alarm(15);
	}
}

void HandlerSIGINT(int sig )
{	
	//DKJR recoit sigint 
	printf("dkjr recoit sigint    \n");
	//pthread_mutex_lock(&mutexGrilleJeu); // peut creer un probleme car lock une fois de trop 
	effacerCarres( 10,(positionDKJr * 2) + 7, 2, 2);
	setGrilleJeu(2, positionDKJr); 
	pthread_mutex_unlock(&mutexGrilleJeu);
	pthread_mutex_unlock(&mutexEvenement);
	printf("fin du sigint \n");
	pthread_exit(0);
}

void HandlerSIGUSR1(int sig )
{
	pthread_mutex_lock(&mutexGrilleJeu);
	int* spe= (int *)pthread_getspecific(keySpec);
	printf(" -- -la position du corbeau est %d   ",*spe);
	effacerCarres( 9 ,(*spe * 2) + 7, 2, 2);
	setGrilleJeu(2, *spe);
	free(spe);
	pthread_mutex_unlock(&mutexGrilleJeu);
	pthread_exit(0);
}
void HandlerSIGUSR2(int sig)
{
	pthread_mutex_lock(&mutexGrilleJeu);
	S_CROCO* spe = (S_CROCO *)pthread_getspecific(keySpec);
	printf(" -- -la position du croco est %d   ",spe->position);
	if(spe->haut==true)
	{
		printf("---true\n");
		effacerCarres(8,(spe->position * 2) + 7, 1, 2);
		setGrilleJeu(1, spe->position);
	}
	else 
	{
		printf("----faux\n");
		effacerCarres(12 ,(spe->position * 2) + 7, 1, 2);
		setGrilleJeu(3, spe->position);
	}
	
	pthread_mutex_unlock(&mutexGrilleJeu);
	pthread_mutex_unlock(&mutexEvenement);
	free(spe);
	pthread_exit(0);
}


void HandlerSIGCHLD(int sig)
{
	pthread_mutex_lock(&mutexGrilleJeu);
	effacerCarres(11 ,(positionDKJr * 2) + 7, 2, 4);
	setGrilleJeu(3, positionDKJr);
	pthread_mutex_unlock(&mutexGrilleJeu);
	pthread_exit(0);
}
void HandlerSIGHUP(int sig )
{
	pthread_mutex_lock(&mutexGrilleJeu);
	effacerCarres(7 ,(positionDKJr * 2) + 7, 2, 2);
	setGrilleJeu(1, positionDKJr);
	pthread_mutex_unlock(&mutexGrilleJeu);
	pthread_mutex_unlock(&mutexEvenement);
	pthread_exit(0);
}