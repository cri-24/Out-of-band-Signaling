#if !defined(WORKER_H)
#define WORKER_H


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
#include <time.h>
#include <sys/time.h>

typedef struct msg_t{
	uint32_t ID_client;
	int ID_server;
	long stima_secret;

}msg_t;


/**
 *@function Worker
 *@brief riceve i secret da un client e calcola la stima del secret in base ai millisecondi che trascorrono tra un msg e il successivo
 *@param s=struct che contiene fd della socket e indice del server
 */
void* Worker(void* s);

#endif