#if !defined(SUPERVISOR_H)
#define SUPERVISOR_H

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
#include <utils.h>
#include <supervisor.h>
#include <signal.h>

//contiene: pipe per comunicare con server e array di pid di server attivati
typedef struct superv{
	int pipe[2];
	pid_t* serv_pid;
}superv_t;

//struct per tokenizzare i messaggi ricevuto dai server tramite socket
typedef struct token{
	char* idclient;
	int server;
	int stima;
	int nserv;
}token_t;

/**
 *@function init_signal
 *@brief configura i segnali SIGINT SIGALRM definendo un handler specifico
 *@param /
 *@preturn void
 */
void init_signal();

/**
 *@function tokenizer
 *@brief tokenizza il messaggio ricevuto dal server in tre parti
 *@param t=struttura con tre campi
 *@param buf= buffer per copiare l'intero messaggio
 *@param dim=dimensione del messaggio
 */
token_t* tokenizer(token_t* t,char* buf, int dim);
/**
 *@function server_create
 *@brief crea i processi figli, cioè i server
 *@param superv=struttura che contiene la pipe per comunicare con il server e pids=array dei pid dei figli
 *@param k=numero di figli da creare
 */
void server_create(superv_t* superv,int k);
/**
 *@function kill_childs
 *@brief uccide i figli dopo aver ricevuto doppio SIGINT
 *@param /
 */
void kill_childs();
/**
 *@function wait_server
 *@brief aspetta che i server lavorino e comunichino la stima in un ciclo infinito
 *@param superv=struttura con pipe e array di pid
 */
void wait_server(int k,superv_t* superv);
/**
 *@function sigint_gest
 *@brief gestisce il segnale SIGINT:stampa la tabella delle stime,manda un allarme di un secondo
 *@param signal=SIGINT

 */
void sigint_gest(int signal);
/**
 *@function sigalarm_gest
 *@brief gestisce il segnale doppio SIGINT, stampa la tabella delle stime e termina il programma
 *@param signal=SIGALRM
 */
void sigalarm_gest(int signal);
/**
 *@function insert_client
 *@brief inserisce un client nella tabella se non esiste, altrimenti aggiorna la stima relativa a quel client con il minimo
 *@param tab= tabella delle stime composta da strutture token
 *@param tok= struttura che contiene quattro campi: idclient,nserv,server,stima
 */
int insert_client(token_t** tab, token_t* tok);
/**
 *@function  create_tab 
 *@brief crea la tabella delle stime da stampare all'arrivo dei segnali e inizializza il campo nserv=0
 *@param /
 */
token_t** create_tab ();

#endif