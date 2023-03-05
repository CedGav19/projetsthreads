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

    pthread_mutex_init(&mutexEvenement, NULL); 

	int evt;
	ouvrirFenetreGraphique();

	// lancement des threads 
	pthread_create(&threadCle, NULL,(void*(*)(void*)) FctThreadCle,NULL);
	pthread_create(&threadEvenements, NULL,(void*(*)(void*)) FctThreadEvenements,NULL);
	pthread_create(&threadDKJr, NULL,(void*(*)(void*)) FctThreadDKJr,NULL);

	//masquage des signaux 
	sigset_t mask; 
	sigemptyset(&mask); 
 	sigaddset(&mask, SIGINT); 
	 sigaddset(&mask, SIGALRM); 
   	sigprocmask(SIG_SETMASK, &mask, NULL); 

	/*afficherCage(2);
	afficherCage(3);
	afficherCage(4);
	afficherRireDK();
	afficherCle(3);
	afficherCroco(11, 2);
	afficherCorbeau(10, 2);*/
	
	effacerCarres(0, 0, 7, 3);

	
	afficherScore(1999);
	while (compteurDeMort<2)
	{
		printf("DKJR est dead \n");
		pthread_join(threadDKJr, (void **)NULL);
		compteurDeMort++;
		afficherEchec(compteurDeMort);
		pthread_create(&threadDKJr, NULL,(void*(*)(void*)) FctThreadDKJr,NULL);
		effacerCarres(11,7, 2, 2);
	}
		pthread_join(threadDKJr, (void **)NULL);


	while(1);

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
   	sigprocmask(SIG_SETMASK, &mask, NULL); 

	int i = 1 ;
	int contexte =1 ; // 1 = aller , 0 = retour 
	 struct timespec t;
 	  t.tv_sec = 0;
  	 t.tv_nsec = 700000000;
	while(1)
	{
		afficherCle(i);
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

		effacerCarres(3,12,2,3);

	}

}

void * FctThreadEvenements (void *)
{

	//masquage des signaux
	sigset_t mask; 
	sigemptyset(&mask); 
 	sigaddset(&mask, SIGINT); 
   	sigprocmask(SIG_SETMASK, &mask, NULL); 

	while (1)
	{
		pthread_mutex_lock(&mutexEvenement);
		evenement= AUCUN_EVENEMENT;
		alarm(1);
	    evenement = lireEvenement();
		printf("passage apres lirevnt\n");
		alarm(0);
		printf("passage apres alarm\n");
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
   	sigprocmask(SIG_SETMASK, &mask, NULL); 



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
												else 
												{
													if(positionDKJr==1)
													{
														setGrilleJeu(3, positionDKJr);
														effacerCarres(11, (positionDKJr * 2) + 7, 2, 2);
														positionDKJr--;
														setGrilleJeu(3, positionDKJr, DKJR);
														afficherDKJr(11, (positionDKJr * 2) + 7, 
														13);
													}
													pthread_mutex_unlock(&mutexGrilleJeu);
													pthread_mutex_unlock(&mutexEvenement);
													nanosleep(&t,NULL);
													pthread_exit(0);
												}
								break;
								case SDLK_RIGHT:
												if (positionDKJr < 7)
												{
													setGrilleJeu(3, positionDKJr);
													effacerCarres(11, (positionDKJr * 2) + 7, 2, 2);
													positionDKJr++;
													setGrilleJeu(3, positionDKJr, DKJR);
													afficherDKJr(11, (positionDKJr * 2) + 7, 
													((positionDKJr - 1) % 4) + 1);
												}											
								break;
								case SDLK_UP:
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
													
												}
											}
											
								break;
							}
			case LIANE_BAS:
							if(evenement==SDLK_DOWN)
							{
								setGrilleJeu(2, positionDKJr);
								effacerCarres(10, (positionDKJr * 2) + 7, 2, 2);

								setGrilleJeu(3, positionDKJr, DKJR);
								afficherDKJr(11, (positionDKJr * 2) + 7, 
								((positionDKJr - 1) % 4) + 1);	
								etatDKJr=LIBRE_BAS;
							}
			break;
			case DOUBLE_LIANE_BAS:
								if(evenement==SDLK_DOWN)
								{
									setGrilleJeu(2, positionDKJr);
									effacerCarres(10, (positionDKJr * 2) + 7, 2, 2);

									setGrilleJeu(3, positionDKJr, DKJR);
									afficherDKJr(11, (positionDKJr * 2) + 7, 
									((positionDKJr - 1) % 4) + 1);	
									etatDKJr=LIBRE_BAS;
								}
								else 
								{
									if(evenement==SDLK_UP)
									{
										setGrilleJeu(2, positionDKJr);
										effacerCarres(10, (positionDKJr * 2) + 7, 2, 2);

										setGrilleJeu(1, positionDKJr, DKJR);
										afficherDKJr(8, (positionDKJr * 2) + 7, 
										7);	
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
												setGrilleJeu(1, positionDKJr);
												effacerCarres(8, (positionDKJr * 2) + 7, 2, 2);

												setGrilleJeu(2, positionDKJr, DKJR);
												afficherDKJr(10, (positionDKJr * 2) + 7, 
												7);
												etatDKJr=DOUBLE_LIANE_BAS;
											}
							break;
							case SDLK_RIGHT:
											if (positionDKJr < 6)
											{
												setGrilleJeu(1, positionDKJr);
												effacerCarres(7, (positionDKJr * 2) + 7, 2, 2);
												positionDKJr++;
												setGrilleJeu(1, positionDKJr, DKJR);
												afficherDKJr(7, (positionDKJr * 2) + 7, 
												((positionDKJr - 1) % 4) + 1);
											}
											else 
											{
												if(positionDKJr==6)
												{
													setGrilleJeu(1, positionDKJr);
													effacerCarres(7, (positionDKJr * 2) + 7, 2, 2);
													positionDKJr++;
													setGrilleJeu(1, positionDKJr, DKJR);
													afficherDKJr(8, (positionDKJr * 2) + 7, 
													7);
													
												}
											}											
							break;
							case SDLK_LEFT:
											if (positionDKJr > 3&& positionDKJr!=7)
											{
												setGrilleJeu(1, positionDKJr);
												effacerCarres(7, (positionDKJr * 2) + 7, 2, 2);
												positionDKJr--;
												setGrilleJeu(1, positionDKJr, DKJR);
												afficherDKJr(7, (positionDKJr * 2) + 7, 
												((positionDKJr - 1) % 4) + 1);
											}
											else
											{
												if (positionDKJr==7)
												{
													setGrilleJeu(1, positionDKJr);
													effacerCarres(8, (positionDKJr * 2) + 7, 2, 2);
													positionDKJr--;
													setGrilleJeu(1, positionDKJr, DKJR);
													afficherDKJr(7, (positionDKJr * 2) + 7, 
													((positionDKJr - 1) % 4) + 1);
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


							break;
						}
			break;
		case LIANE_HAUT:
						setGrilleJeu(0, positionDKJr);
						effacerCarres(6, (positionDKJr * 2) + 7, 2, 2);
						setGrilleJeu(1, positionDKJr, DKJR);
						afficherDKJr(7, (positionDKJr * 2) + 7, 
						((positionDKJr - 1) % 4) + 1);
						etatDKJr=LIBRE_HAUT;
		break;
		}
		pthread_mutex_unlock(&mutexGrilleJeu);
		pthread_mutex_unlock(&mutexEvenement);
	}
	pthread_exit(0);	
}
void HandlerSIGQUIT(int sig )
{
	//printf("dkjr recoit sigquit\n");
}
void HandlerSIGALRM(int s )
{
	evenement=AUCUN_EVENEMENT;
	printf("PASSAGE DANS SIGALRM avec evenement = %d\n",evenement);
	// peut etre faire un jump 
}