
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
#include <sys/types.h>
#include <sys/un.h>
#include <string.h>
#include <client.h>
#include <stdint.h>
#include <utils.h>
#include <inttypes.h>

int main (int argc, char* argv[]){

	if(argc != 4){
		printf("Usage: %s <p> <k> <w> \n",argv[0]);
		exit(EXIT_FAILURE);
	}

	int p,k,w; 
	int* server_to_conn; //array di server a cui connettermi
	param_t* param;
 
	p = atoi( argv[1] ); //tra 1 e k 
	k = atoi( argv[2] ); //numero di server attivi
	w = atoi( argv[3] ); //>3p: > del triplo di p

	//inizializzo la struttura param e controllo la validità dei parametri
	param = init_check_param(p,k,w);
	
	//creo e inizializzo la struct client con ID e secret
	client_t* client = create_client();
	client = init_client(client);

	printf("CLIENT %" PRIx32 " SECRET %d\n",client->ID,client->secret);
	fflush(stdout);
	//array di fd dei server di dim p
	CHECK_EQ(server_to_conn,calloc((param->p),(sizeof(int))),NULL,"calloc");

	//scelgo a caso i server con cui mi connetterò
	choose_server(server_to_conn,param);

	int* arr_fd;
	CHECK_EQ(arr_fd,calloc((param->p),sizeof(int)),NULL,"CALLOC");

	//inizio la connessione 
	arr_fd= start_connection(arr_fd,server_to_conn,param);
	send_ID(arr_fd,client,param);
	printf("CLIENT %" PRIx32" DONE \n",client->ID);

	//libero memoria ed esco
	free(client);
	free(server_to_conn);
	free(param);
	free(arr_fd);
	fflush(stdout); 
	return 0;


}