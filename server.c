
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
#include <signal.h>



int main (int argc, char* argv[]){

	if(argc != 3){
		printf( "Usage: %s <indice_server> <fd_pipe>\n",argv[0]);
		exit(EXIT_FAILURE);
	}
	
	
	init_signals();
	
	server_t* server = create_server();
	init_server(server,atoi(argv[1]),atoi(argv[2]));
	printf("SERVER %d ACTIVE \n",server->i);
	//avvio server
	start_server(server);
	
	return 0;


}	
