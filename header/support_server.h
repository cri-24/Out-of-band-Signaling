#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <client.h>
#include <utils.h>

server_t* create_server(){
	server_t* server;
	CHECK_EQ(server, (server_t*)malloc(sizeof(server_t)), NULL, "Creazione server fallita");
}


void init_server(server_t* server,int id_serv, int fd_pipe){




}