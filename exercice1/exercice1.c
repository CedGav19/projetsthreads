#include <pthread.h> 
#include <stdio.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h> 

pthread_t threadHandle1 ,threadHandle2,threadHandle3,threadHandle4 ; 

struct paramEff
{
	char mot[20];
	char fichier[50];
	int tabulation ;

};

void * fctThread(struct paramEff * param); 

int main() { 

	struct paramEff param1 , param2 ,param3 , param4 ;
	int *retThread1, ret ,*retThread2 ,*retThread3 ,*retThread4 ;

strcpy(param1.mot , "Garage");
strcpy(param2.mot , "Instance");
strcpy(param3.mot , "Client");
strcpy(param4.mot , "Vendeur");

strcpy(param1.fichier , "Garage.cpp");
strcpy(param2.fichier , "Garage.cpp");
strcpy(param3.fichier , "Garage.cpp");
strcpy(param4.fichier , "Garage.cpp");

param1.tabulation=0 ;
param2.tabulation=1 ;
param3.tabulation=2 ;
param4.tabulation=3 ;


 ret = pthread_create(&threadHandle1, NULL,(void*(*)(void*)) fctThread, &param1);
 ret = pthread_create(&threadHandle2, NULL,(void*(*)(void*)) fctThread, &param2);
 ret = pthread_create(&threadHandle3, NULL,(void*(*)(void*)) fctThread, &param3);
 ret = pthread_create(&threadHandle4, NULL,(void*(*)(void*)) fctThread, &param4);

  puts("Thread secondaires lance !"); 
  puts("Attente de la fin des thread secondaire"); 
  ret = pthread_join(threadHandle1, (void **)&retThread1); 
    ret = pthread_join(threadHandle2, (void **)&retThread2); 
    ret = pthread_join(threadHandle3, (void **)&retThread3); 
    ret = pthread_join(threadHandle4, (void **)&retThread4); 
  puts("Fin du thread secondaire 4 ");

  printf("nombre de retour du premier thread :  %d\n" ,*retThread1);
   printf("nombre de retour du 2e thread :  %d\n" ,*retThread2);
    printf("nombre de retour du 3e thread :  %d\n" ,*retThread3);
     printf("nombre de retour du 4e thread :  %d\n" ,*retThread4);

    free(retThread1);
     free(retThread2);
      free(retThread3);
       free(retThread4);
    puts("Fin du thread principal");
 } 

 void * fctThread (struct paramEff* param) { 
 	int fd, nb  ,i , j; 
 	int *nombre ;
 	nombre = (int *) malloc(sizeof(int) );
 	*nombre = 0;
 	j=0 ;

 	char  tmpchar[20];

 	if((fd = open (param->fichier, O_RDONLY ))==-1)
 	{
 		 puts("err ouv du fichier");
 		 exit(1);
 	}
 	else 
 	{
 	 	nb = lseek (fd ,0 ,SEEK_END);
 		close(fd);
		printf("nb = %d\n",nb);

	for (i=0 ; i<nb ; i++)
	{
		//printf("%d\n",i);
 		if ((fd = open (param->fichier, O_RDONLY ))==-1)
 		{
 			 puts("err ouv du fichier");
 		 	exit(1);
 		}
 		else 
 		{
 			j=0;
	 		while(j<param->tabulation)
	 		{
	 			printf("\t");
	 			j++;
	 		}
	 		printf("*\n");

	 		lseek (fd ,i,SEEK_SET);
	 		if (read(fd , tmpchar ,strlen(param->mot))==-1)
	 		{
			 puts("err  du read");
			 exit(1);
	 		}
	 		tmpchar[strlen(param->mot)] = 0;
 			if(strcmp(tmpchar,param->mot)==0)
 			{
 				(*nombre) ++ ;
 			}
			close(fd);	
 		}
 		
 	}
 	}
 	pthread_exit(nombre);


 }
