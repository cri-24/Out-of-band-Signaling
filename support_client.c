

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
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <client.h>
#include <utils.h>
#include <stdint.h>
#include <inttypes.h>
#include <endian.h>
#include <time.h>
#include <arpa/inet.h>



param_t* init_check_param(int p,int k,int w){

	//inizializzo la struct dei parametri: p,k,w
	param_t* param;
	CHECK_EQ(param, ((param_t*)calloc(1,sizeof(param_t))),NULL,"calloc");
	param->p = p; //tra 1 e k
	param->k = k; //numero di server attivi
	param->w = w; //>3p: > del triplo di p

	//controllo i parametri
	if( param->p<1 || param->p>=k ){
		printf( "<p> deve essere un intero tra 1 e %d\n",k);
		exit(EXIT_FAILURE);
	}	

	if( param->w <= 3*(param->p) ){
		printf("<w> deve essere un intero maggiore di %d\n", 3*p);
		exit(EXIT_FAILURE);
	}

	return param;
}
client_t* create_client(){
	client_t* client;
	CHECK_EQ(client, (client_t*)calloc(1,sizeof(client_t)), NULL, "Creazione client fallita");
	return client;
}

uint64_t XOR(uint32_t a, uint32_t b){
	return a ^ b;
}

uint64_t AND(uint32_t a, uint32_t b){
	return a & b;
}
//generatore pseudo-casuale di numeri a 64bit
uint64_t Rand32(){
	uint32_t val = 0;
	uint32_t hex = 0xFFFF;

	//intero con segno a 32 bit
	uint32_t and;
	and =  AND((uint16_t)rand() , hex);
	val = XOR(val,and);
	and = AND ((uint16_t)rand() , hex)<<16;
	val = XOR(val,and);

	return val;
}


//inizializzo la struttura client con i campi ID e secret
client_t* init_client(client_t* client){
	if(client==NULL) exit(EXIT_FAILURE);
	int esito;
	struct timeval t;
	CHECK_EQ(esito,gettimeofday(&t,NULL),-1,"gettimeofday");
	//rinizializza il seme e grazie a time(NULL) assicura che il seme sia sempre diverso per ottenere numeri sempre diversi
	srand(t.tv_usec ^ t.tv_sec);
	client->ID= Rand32();
	client->secret= ( rand()%3000 ) + 1;
	return client;

}

int cerca(int* arr, int p, int elem){
	if(arr==NULL || p<0 || elem<0) exit(EXIT_FAILURE);
	int i=0;
	int trovato=0;

	while(i<p && !trovato){
		if(arr[i]!=elem){
			i++;
		}
		else trovato=1;
	}
	return trovato;
}

//scelgo tra i k server, solo p server in modo casuale
void choose_server(int* scelti_rand,param_t* param){
	if(scelti_rand==NULL || param==NULL) exit(EXIT_FAILURE);
	int serv;
	int i=0;
	while(i<(param->p)){
		serv = (rand()%(param->k))+1;
		
		if(cerca(scelti_rand,param->p,serv)==0){
			
			scelti_rand[i]=serv;
			i++;

		}
	}
}



//mi connetto a uno dei p server scelti precedentemente tra i k, in modo random
int* start_connection(int* arr_fd,int* server_to_conn,param_t* param){
	if(arr_fd==NULL || param==NULL || server_to_conn==NULL) exit(EXIT_FAILURE);
	
	//arr_fd: array di fd dei socket per ogni comunicazione client-server
	
	for(int i=0; i<(param->p); i++){
	
		struct sockaddr_un sa;
		memset(&sa,0,sizeof(sa));
		sa.sun_family = AF_UNIX;

		sprintf(sa.sun_path, "OOB-server-%d", server_to_conn[i]);
		//richiesta di connessione al server
		CHECK_EQ(arr_fd[i], socket(AF_UNIX,SOCK_STREAM,0), -1, "socket fallita");	
		while((connect(arr_fd[i],(struct sockaddr *)&sa, sizeof(sa)))==-1){
			if(errno != ENOENT)
				printf("errno != ENOENT\n");
			sleep(1);
			
		}

	}
	return arr_fd;
}



void send_ID(int* arr_sock,client_t* client, param_t* param){
	if(arr_sock==NULL || param==NULL || client==NULL) exit(EXIT_FAILURE);
	
	uint32_t id;
	//converto in network byte order prima di inviare
	id = htonl(client->ID);
	struct timespec req,rem;
	int nbyte=-1;
	//secret in secondi
	req.tv_sec = client->secret/1000;
	//in nanosec
	req.tv_nsec = (client->secret%1000)*1000000;
	int esito;
	int r=-1;
	//invia in totale w messaggi
	for(int i=0; i<param->w; i++){
		//dorme secret millisecondi
		CHECK_EQ(esito,nanosleep(&req,&rem),-1,"nanosleep");
		r=(rand()%param->p);
		//invia il proprio ID dopo secret millisecondi a un casuale server
		CHECK_EQ(nbyte,write(arr_sock[r],&id,sizeof(uint32_t)),0,"send");
	}

	//protocollo di temrinazione
	id=0;
	int esito1;
	for(int i=0; i<param->p;i++){
		CHECK_EQ(nbyte,write(arr_sock[i],&id,sizeof(uint32_t)),0,"send");
		CHECK_EQ(esito1,close(arr_sock[i]),-1,"close socket fallita_client");
	}
	
}
