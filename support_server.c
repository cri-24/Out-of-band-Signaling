
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

static volatile sig_atomic_t sig_term=0;

void signal_handler(int signal){
	sig_term=1;
	
}
void cleanup(char* sockname ){
	if(unlink(sockname) == -1 && errno != ENOENT){
		perror("unlink fallita");
		exit(EXIT_FAILURE);
	}

}


void init_signals(){
	int esito;
	struct sigaction s1,s2;
	//azzero le strutture
	memset(&s1,0,sizeof(s1));
	memset(&s2,0,sizeof(s2));
	
	//insieme dei segnali bloccati 
	sigemptyset(&s1.sa_mask);
	sigemptyset(&s2.sa_mask);
	s1.sa_handler=SIG_IGN;
	s2.sa_handler=signal_handler;
	CHECK_EQ(esito,sigaction(SIGINT,&s1,NULL),-1,"sigaction"); 
	CHECK_EQ(esito,sigaction(SIGALRM,&s1,NULL),-1,"sigaction"); 
	CHECK_EQ(esito,sigaction(SIGTERM,&s2,NULL),-1,"sigaction"); 

}


server_t* create_server(){
	server_t* server;
	CHECK_EQ(server, (server_t*)calloc(1,sizeof(server_t)), NULL, "Creazione server fallita");
	return server;
}


void init_server(server_t* s,int id_serv, int fd_pipe){
	if(s==NULL || id_serv<0 || fd_pipe<0) exit(EXIT_FAILURE);
	s->i = id_serv;
	s->fdpipe = fd_pipe;

}



void start_server(server_t* s){
	if(s==NULL) exit(EXIT_FAILURE);
	int listen_fd;
	int notused;
	char sockname[30];
	struct sockaddr_un sa; 
	memset(&sa,0,sizeof(sa));
	sa.sun_family = AF_UNIX;
	//costruisco il socket_name
	strncpy(sockname, SOCK_NAME, strlen(SOCK_NAME));
	sprintf(sockname, SOCK_NAME"%d", s->i);
	
	//controllo che non esista già il socket con quel nome
	cleanup(sockname);

	CHECK_EQ(listen_fd, socket(AF_UNIX,SOCK_STREAM,0), -1, "socket fallita");
	strcpy(sa.sun_path,sockname);
	CHECK_EQ(notused, bind(listen_fd, (struct sockaddr*) &sa, sizeof(sa)),-1, "bind fallita");
	CHECK_EQ(notused, listen(listen_fd,SOMAXCONN),-1,"listen fallita");
	
	while(1){
		pthread_t th;
		int conn_fd;
		if((conn_fd= accept(listen_fd, NULL, 0)) == -1){
			if(errno==EINTR){
				if(sig_term) break;
				else{ perror("server accept fallita");
				exit(EXIT_FAILURE);}
			}
		}
		printf("SERVER %d CONNECTED FROM CLIENT\n",s->i);
		s->fd=conn_fd;
		arg_t* arg;
		CHECK_EQ(arg, (arg_t*)calloc(1,sizeof(arg_t)), NULL, "calloc");
		arg->fd=conn_fd;
		arg->i=s->i;
		arg->fdpipe=s->fdpipe;

		//creo e lancio il thread worker in modalità detach
		CHECK_NEQ_th(pthread_create(&th, NULL, &Worker, arg),0,"pthread create fallita");
		CHECK_NEQ_th(pthread_detach(th),0,"pthread_detach fallita");
		
	}
	free(s);
	fflush(stdout);
	exit(EXIT_SUCCESS);
}
