
/* 	PROGETTO SOL "OUT-OF-BANDING" 2018/2019
*	Autore: Cristiana Angiuoni 
*	Matricola: 546144
*	Il codice è stato scritto interamente dalla sottoscritta.
*/



#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <utils.h>
#include <supervisor.h>
#include <signal.h>



int main (int argc,char** argv){


	int k;

	if(argc!=2){
		printf("Usage: %s <k>",argv[0]);
		exit(EXIT_FAILURE);
	}

	k=atoi(argv[1]);
	printf("SUPERVISOR STARTING %d\n",k);

	//specifico come gestire i segnali con sigaction
	init_signal();

	//imposta la bufferizzazione dello stream e serve nel caso lo stndout sia un file:
	//così output di server e supervisor non si mischiano sulla stessa riga
	//IOLBF: definisce un I/O con bufferizzazione a linea
	int esito=setvbuf(stdout,NULL,_IOLBF,BUFSIZ);
	if (esito==-1) {
		exit(EXIT_FAILURE);
	}
	//alloco memoria per la struttura del supervisore e i suoi campi
	superv_t* superv;
	CHECK_EQ(superv,(superv_t*)calloc(1,sizeof(superv_t)),NULL,"calloc");
	
	//creo un array di k+1 pid, uno per ogni server
	CHECK_EQ(superv->serv_pid,calloc(k+1,sizeof(int)),NULL,"calloc");

	server_create(superv,k);
	wait_server(k,superv);

	//libero la memoria allocata
	free(superv->serv_pid);
	free(superv);
	return 0;
}