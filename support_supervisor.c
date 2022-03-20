
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
#include <server.h>
#include <utils.h>
#include <supervisor.h>
#include <unistd.h>
#include <signal.h>


#define EXEC_SERVER "./server"
#define NCLIENT 20
//variabili volatile per gestire i segnali
static volatile sig_atomic_t sig_print=0;//stampa tab_stime se =1
static volatile sig_atomic_t sig_alarm=0;
static volatile sig_atomic_t sig_exit=0;

pid_t* pids;
token_t** tab;


//stampa le stime finali sia su stndout sia stderr
void print_signal(int fd){
	
		for(int i=0; i<NCLIENT; i++){
				int esito;
				char buf[64];
				if(tab[i]->nserv!=0){
					sprintf(buf,"SUPERVISOR ESTIMATE %d FOR %s BASED ON %d\n",tab[i]->stima,tab[i]->idclient,tab[i]->nserv);
					int n=strlen(buf);
					CHECK_EQ(esito,write(fd,&buf,n),-1,"write");	
				}
		}
	
}

//gestione asincrona dei segnali
//se sig_alarm=1 allora è passato più di un secondo dall'ultimo SIGINT e devo terminare il supervisor
void sigint_gest(int signal){
	if(sig_alarm==0){
		
		sig_print=1;
		sig_alarm=1;
		alarm(1);
	}
	else{
	sig_exit=1;
	}	

}

//è passato un secondo dall'ultimo SIGINT allora non esco
void sigalarm_gest(int signal){
	sig_alarm=0;
}

void init_signal(){

//inizializza sigaction per SIGINT e SIGALARM
	struct sigaction s1,s2;
	//azzero le strutture
	memset(&s1,0,sizeof(s1));
	memset(&s2,0,sizeof(s2));
	//insieme dei segnali bloccati durante l'esecuzione dell'handler 
	sigemptyset(&s1.sa_mask);
	sigemptyset(&s2.sa_mask);
	//s1.sa_flags=0;
	//s2.sa_flags=0;
	//SIGINT: di default termina il processo, ma qui esegue il sigint_gest
	s1.sa_handler=sigint_gest;
	int esito;
	CHECK_EQ(esito,sigaction(SIGINT,&s1,NULL),-1,"sigaction"); 
	//SIGALRM: sveglia mandata dal sistema operativo
	s2.sa_handler=sigalarm_gest;
	CHECK_EQ(esito,sigaction(SIGALRM,&s2,NULL),-1,"sigaction"); 
}



void server_create(superv_t* superv,int k){
	if(superv==NULL || k<0) exit(EXIT_FAILURE);
	superv->serv_pid[k]=-1;//per la terminazione

	int notused;
	//argomenti della execl
	char num_server[10];
	char fd_pipe[10];
	//array di pid
	CHECK_EQ(pids,(pid_t*)calloc(k+1,sizeof(pid_t)),NULL,"calloc");
	pids[k]=-1;
	
	//creo la pipe
	CHECK_EQ(notused,pipe(superv->pipe),-1,"pipe");
	for(int i=0; i<k; i++){
		pid_t pid;
		//creo i processi figli
		CHECK_EQ(pid,fork(),-1,"fork");

		//se è il figlio(server) con la execl sostituisco il codice ./server
		if(pid==0){
			//tengo la pipe aperta in sola scrittura perchè sono il figlio
			CHECK_EQ(notused,close(superv->pipe[0]),-1,"close"); 
			//preparo i parametri per execl del server: devono essere char* per execl
			sprintf(num_server,"%d",i+1);
			//fd della pipe in cui il server scriverà la stima
		    sprintf(fd_pipe,"%d",superv->pipe[1]);
			//mando in esecuzione il server figlio
			CHECK_EQ(notused,execl(EXEC_SERVER,"server",num_server,fd_pipe,NULL ),-1,"execl ./server");
			
		}
		else if(pid==-1){ //fork fallita
			perror("fork");
			exit(EXIT_FAILURE);
		}
		else{ //il padre
			superv->serv_pid[i]=pid;
			pids[i]=pid;
		}

	}
	//chiudo in scrittura
	CHECK_EQ(notused,close(superv->pipe[1]),-1,"close pipe");
	//esco dal supervisore e elimino i server creati precedentemente mandando SIGTERM, registrando la funzione kill_childs in atexit
	if(atexit(kill_childs)!=0){
		printf("atexit fallita \n");
		exit(EXIT_FAILURE);
	}

}



//tokenizzo il messaggio ricevuto dal server:versione rientrante
token_t* tokenizer(token_t* tok,char* buf, int dim){

	char buf_copy[64];
	memset(buf_copy,0,64);
	strncpy(buf_copy,buf,64);
	char* save;
	//tokenizzo e riempo la struct tok
	strncpy(tok->idclient,strtok_r(buf_copy," ",&save),9);
	
	char* stima=strtok_r(NULL," ",&save);
	char* server=strtok_r(NULL, " ",&save);
	
	//converto da stringa a int
	tok->stima=atoi(stima);
	tok->server=atoi(server);

	return tok;
}

token_t** create_tab (){

	//creo la tabella di token per salvare e aggiornare le stime da stampare
	CHECK_EQ(tab,(token_t**)calloc(NCLIENT,sizeof(token_t*)),NULL,"calloc");

	//alloco e inizializzo tab->nserv=0
	for(int i=0; i<NCLIENT;i++){
		CHECK_EQ(tab[i],(token_t*)calloc(1,sizeof(token_t)),NULL,"calloc");
		CHECK_EQ(tab[i]->idclient,(char*)calloc(9,sizeof(char)),NULL,"calloc");
		tab[i]->nserv=0;
	}
	return tab;
}


//inserisco il client se non esiste, altrimenti aggiorno la stima con il minimo
int insert_client(token_t** tab, token_t* tok){
	if(tab==NULL || tok ==NULL) exit(EXIT_FAILURE);
	int trovato=0;
	int i=0;
				while(i<NCLIENT && !trovato ){
					if(tab[i]->nserv==0){

						strncpy(tab[i]->idclient,tok->idclient,9);
						tab[i]->stima=tok->stima;
						tab[i]->nserv++;
						trovato=1;
					}
					else if( strncmp(tab[i]->idclient,tok->idclient,9)==0){
						if(tab[i]->stima>=tok->stima){
							tab[i]->stima=tok->stima;	
						}
						trovato=1;
						tab[i]->nserv++;

					}
					i++;

				}
				return i;
}

//legge i messaggi sulla pipe e nel caso in cui arriva SIGINT attiva la stampa
void wait_server(int k,superv_t* superv){
	if(superv==NULL) exit(EXIT_FAILURE);
	int nbyte;
	char* buf;
	CHECK_EQ(buf,(char*)calloc(64,sizeof(char)),NULL,"calloc");
	token_t* tok;
	//per tokenizzare il msg che arriva dal server
	CHECK_EQ(tok,(token_t*)calloc(1,sizeof(token_t)),NULL,"calloc");
	CHECK_EQ(tok->idclient,(char*)calloc(9,sizeof(char)),NULL,"calloc");
	int esito;
	tab=create_tab();
	if(tab==NULL) exit(EXIT_FAILURE);
	int size=sizeof(char)*64;
	
		//finchè c'è almeno un descrittore aperto in scrittura
		while((nbyte= read(superv->pipe[0],buf,size))){
			
			//se errno=EINTR posso aver ricevuto SIGINT o SIGALARM quindi non voglio terminare il processO
			if(nbyte==-1 && errno!=EINTR){ 
				perror("read pipe");
				exit(EXIT_FAILURE);
			}
		
			//ho finito di leggere tutto il msg
			if(nbyte==size){
				//tokenizzo il messaggio
				tok=tokenizer(tok,buf,size);
				printf("SUPERVISOR ESTIMATE %d FOR %s FROM %d\n",tok->stima,tok->idclient,tok->server);
				//inserisco il client se non esiste, altrimenti aggiorno la stima 
				esito=insert_client(tab,tok);
				if (esito==-1) printf("errore di inserimento client");
				size=sizeof(char)*64;
				
			} 

			//ho letto almeno un byte, ma sono stato interrotto da un segnale
			else if(nbyte > 0) {
				//riprendo la lettura da dove mi ero fermato	
				size=size-nbyte;
				buf+=nbyte;
			}
			
			if(sig_print){
				print_signal(2);
				sig_print=0;
			}

			if(sig_exit){
				CHECK_EQ(esito,close(superv->pipe[0]),-1,"close pipe lettura");
				print_signal(1);


				printf("SUPERVISOR EXITING\n");
				free(buf);
				free(tok->idclient);
				free(tok);
				fflush(stdout);
				exit(EXIT_SUCCESS);
			}
		}
		
	}
	
	





//invia un segnale SIGKILL ai server che sono stati creati da supervisor. E' stata registrata all'inizio con la aexit e viene eseguita all'uscita.
void kill_childs(){
	//libero la memoria allocata
		for(int i=0; i<NCLIENT; i++){
			if(tab[i]->nserv!=0){
				free(tab[i]->idclient);
				free(tab[i]);
			}
		}
		free(tab);
		
	
	int i=0;
	char socket[20];
	//terminaotore dei pid ==-1
	while(pids[i]!=-1){
		sprintf(socket,"OOB-server-%d",i+1);
		//elimino il socket creato dal server 
		//ENOENT:nessun file o directory
		if(unlink(socket)==-1 && errno!=ENOENT)
			perror(socket);
		//se errno==ESCH: nessun processo, allora il server è già stato chiuso
		//mando SIGTERM ai server 
		if(kill(pids[i],SIGTERM)==-1 && errno != ESRCH){
			perror("kill");
		}
		i++;	
	}
	
	free(pids);

}
