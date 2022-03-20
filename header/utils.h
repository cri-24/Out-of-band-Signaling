#if !defined(UTILS_H)
#define UTILS_H

/* 	PROGETTO SOL "OUT-OF-BANDING" 2018/2019
*	Autore: Cristiana Angiuoni 
*	Matricola: 546144
*	Il codice è stato scritto interamente dalla sottoscritta.
*/


/* 
*contiene: -macro per gestire errori sia per i processi che per i thread		  
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>

#define MAX_LEN_SECRET 3000
#define SOCK_NAME "./OOB-server-"


#define CHECK_EQ(r,x,val,str) \
		if((r=x)==val){			\
			perror(str);	\
			exit(errno);     \
		}


#define CHECK_NEQ(r,x,val,str) \
		if((r=x)!=val){			\
			perror(str);	\
			exit(errno);     \
		}

#define CHECK_EQ_th(x,val,str) \
		if((x)==val){			\
			perror(str);	\
			pthread_exit(NULL);     \
		}

#define CHECK_NEQ_th(x,val,str) \
		if((x)!=val){			\
			perror(str);	\
			pthread_exit(NULL);     \
		}

#define PRINT(x,str)\
		if(x==-1)\
			return;\

#endif /*UTILS_H*/