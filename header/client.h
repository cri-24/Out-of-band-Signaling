#if !defined(CLIENT_H)
#define CLIENT_H

/* 	PROGETTO SOL "OUT-OF-BANDING" 2018/2019
*	Autore: Cristiana Angiuoni 
*	Matricola: 546144
*	Il codice è stato scritto interamente dalla sottoscritta.
*/

 
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <stdint.h>
#include <inttypes.h>



//struttura che client contiene ID e secret e rappresenta un generico client

typedef struct client{
	uint32_t ID;
	int secret;
}client_t;

//struttura che contiene i parametri passati da linea di comando: p,k,w

typedef struct param{
	int p;
	int k;
	int w;
}param_t;


/**
 *@function init_check_param
 *@brief inizializza la struttura con i parametri passati da linea di comando e controlla che siano validi
 *@param p=server da scegliere
 *@param k=numero di server attivi
 *@param w=numero di messaggi totali da mandare
 */
param_t* init_check_param(int p,int k,int w);

/**
 *@function create_client
 *@brief alloca memoria per la struct del client
 */
client_t* create_client();

/**
 *@function init_client
 *@brief inizializza la struct del cient
 *@param client=struct da inizializzare
 */

client_t* init_client(client_t* client);
/**
 *@function Rand32()
 *@brief generatore pseudo-casuale di numeri a 32bit

 */

uint64_t Rand32();

/**
 *@function cerca
 *@brief cerca nell'array un server a cui connettersi, se non già scelto alla precedente iterazione
 *@param arr[] = dove cercare
 *@param p=quanti sceglierne
 *@param elem = indice già scelto:per evitare di scegliere sempre lo stesso
 */
int cerca(int arr[], int p, int elem);
/**
 *@function choose_server
 *@brief sceglie p server a cui il client si connetterà tra i k attivi
 *@param server_to_conn = array di indici tra cui sceglie
 *@param param = struct per conoscere il numero di server da scegliere random
 */
//sceglie <p> server tra i <k> server attivi in modo casuale
void choose_server(int* server_to_conn,param_t* param);

/**
 *@function start_connection
 *@brief inizia la connessione tra un client e i <p> server
 *@param arr_fd = array di fd 
 *@param server_to_conn = array di indici di server a cui connettermi
 *@param param = struct con i parametri passati al programma
 */
//connessione tra un client e i <p> server
int* start_connection(int* arr_fd,int* server_to_conn,param_t* param);

/**
 *@function send_ID
 *@brief invia l'ID al server a cui si connette
 *@param arr = array di fd tra cui sceglie random uno alla vola in un ciclo
 *@param param = struct di parametri del programma
 */
void send_ID(int* arr,client_t* client, param_t* param);


#endif 