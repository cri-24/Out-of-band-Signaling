#if !defined(SERVER_H)
#define SERVER_H

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
#include <client.h>
#include <utils.h>
#include <pthread.h>
#include <server.h>


//struttura che rappresenta il server
typedef struct server{
	int fd;
	int i;
	int fdpipe;

}server_t;
//struttura che passo al worker 
typedef struct arg{
	int i;
	int fd;
	int fdpipe;
}arg_t;
/**
 *@function create_server
 *@brief crea la struttura del server
 */
server_t* create_server();
/**
 *@function 
 *@brief
 *@param server= struct 
 *@param connfd = fd della socket
 *@param i= indece del server
 */
//inzializzo la struttura del server
void init_server(server_t* server, int connfd, int i);

/**
 *@function start_server
 *@brief crea un thread per ogni client che si connette
 *@param s = struct server 
 */
void start_server(server_t* s);
/**
 *@function init_signals
 *@brief inizializza i segnali che arriveranno ai server e ai thread
 *@param /
 */
void init_signals();

#endif