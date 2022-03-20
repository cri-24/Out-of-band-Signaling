

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
#include <sys/socket.h>
#include <server.h>
#include <utils.h>
#include <worker.h>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <signal.h>



//funzione che esegue il thread per trasformare in millisec
long trasf_millisec(struct timespec start, struct timespec stop){
	//trasformo i sec in millisec (molt per 10^3)
	long millisec = (stop.tv_sec - start.tv_sec)*1000; 
	//trasformo i nanosec in millisec(divido per 10^6)
	millisec += (stop.tv_nsec - start.tv_nsec)/1000000;
	return millisec;
}


void* Worker(void* arg){
	if(arg==NULL) pthread_exit(NULL);
	
	arg_t* s = (arg_t*) arg;
	uint32_t ID; 
	msg_t* msg;
	CHECK_EQ(msg, ((msg_t*)calloc(1,sizeof(msg_t))),NULL,"calloc");
	long stima=-1;
	//per stimare il secret tra il primo e il secondo messaggio in nanosec
	struct timespec start;
	struct timespec stop;
	struct timespec time_init;
	long arrival_time;
	int byte;
	int check=-1;
	msg->stima_secret=-1;
	int esito;
	//prendo il tempo prima di leggere per calcolare il tempo di arrivo 
	CHECK_EQ_th(clock_gettime(CLOCK_REALTIME,&time_init),-1,"clock_gettime");	
		 	
	//legge da s->fd l'ID mandato dal client,lo salva in ID e lo converte
	
	while(( byte = read(s->fd, &ID, sizeof(uint32_t)))>0){
		
		if(ID==0){ //terminazione: il client scrive 0 sul fd
			if(msg->stima_secret != -1) {
				msg->ID_server = s->i;
				printf("SERVER %d CLOSING %" PRIX32 " ESTIMATE %ld\n", msg->ID_server,msg->ID_client,msg->stima_secret);
				break;
			}//ho ricevuto un solo messaggio, quindi non invio niente
			else pthread_exit(NULL);
		}

		//ci entro solo per il primo messaggio  e inizializzo start
		if(check==-1){
			CHECK_EQ_th(clock_gettime(CLOCK_REALTIME,&start),-1,"clock_gettime");	
		 	check=1;

		}

		else{ //è il secondo messaggio e calcolo la differenza tra stop e start
			CHECK_EQ_th(clock_gettime(CLOCK_REALTIME,&stop),-1,"clock_gettime");	
			//tra un messaggio e il successivo
			stima=trasf_millisec(start,stop);
			start=stop;
			//prendo il minimo
			if(msg->stima_secret==-1 || stima < msg->stima_secret){
				msg->stima_secret=stima;	
			}
		}
		//arrival_time è la differenza tra il tempo iniziale e l'ultimo preso
		arrival_time=trasf_millisec(time_init,start);
		//converto in host to order
		msg->ID_client=ntohl(ID);
		printf("SERVER %d INCOMING FROM %" PRIx32 " @ %ld\n", s->i,ID,arrival_time);
		
	}
	//controllo che la read non sia fallita
	CHECK_EQ_th(byte, -1,"read fallita");

	
	//se la stima è maggiore di 3000 di sicuro è almeno il doppio, allora divido
	int div=2;
	while(msg->stima_secret > 3000){
			if(msg->stima_secret/div < 3000){
				msg->stima_secret=msg->stima_secret/div;
			}
			div++;
	}


	//preparo l'invio della stima migliore
	char* buf;
	CHECK_EQ(buf,(char*)calloc(64,sizeof(char)),NULL,"calloc");
	sprintf(buf,"%x %ld %d",msg->ID_client,msg->stima_secret,msg->ID_server);

	//scrivo sulla pipe in modo atomico, quindi non uso lock
	CHECK_EQ(byte,write(s->fdpipe, buf,sizeof(char)*64),-1,"write on pipe" );
	
	//ogni thread chiude il file descriptor della comunicazione con un client
	CHECK_EQ(esito,close(s->fd),-1,"close socket fallita_worker");
	
	//libero memoria ed esco
	free(buf);
	free(msg);
	fflush(stdout);
	pthread_exit(NULL);


}
