/********************************************************************************************************************
Author : Chinmoy Mohapatra (MT2016505)

Title:
This program is server program to paly the sound recieved from client via socket

compile: gcc play_server.c -o server.c -lpulse-simple
execute: ./play_server 

********************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pulse/simple.h>
#include <pulse/error.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>


#define BUFF_SIZE 1024

int ret,socket_id,client_id;
struct sockaddr_in server_address;
pa_simple * play = NULL;


//signal handler to exit the program
void handler(int signo)
{
	char response;
	if (signo==SIGINT)
	{
		//ask user whether tp disconnect or not 
		printf("Disconnect the server.?(Y/n)\n");
		scanf("%c",&response);

		if (response == 'Y' | response == 'y')
		{
			//close or free audio resources
			pa_simple_free(play);

			//close or free scoket resources
			close(socket_id);
			exit(0);
		}
	}
}
//create scoket 
void socket_create(char * IP,int PORT)
{
	socket_id = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (socket_id == -1)
	{
		perror("ERROR creating socket:");
		exit(0);
	}
	//assign IP and port to the socket
	bzero(&server_address,sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr(IP);
	server_address.sin_port = htons(PORT);
	bzero(server_address.sin_zero,sizeof(server_address.sin_zero));
}

int main(int argc, char * argv[])
{

	if (signal(SIGINT,handler)==SIG_ERR)
	{
		printf("Can't catch the signal\n");
	}

	//check for enough parameters
	if (argc < 2)
	{
		printf("Not enough parameter. Please provide the IP address and port no to be used.\n");
		exit(0);
	}

	/*********************************socket********************************/
	char * IP = argv[1];
	int PORT = atoi(argv[2]);

	//create-bind-listen-accept
	//create scoket
	socket_create(IP,PORT);

	//bind host with the scoket
	ret = bind(socket_id,(struct sockaddr *) &server_address,sizeof(server_address));
	if (ret==-1)
	{
		perror("ERROR binding to scoket:");
		exit(0);
	}

	//listen at socket
	ret = listen(socket_id,1);
	if (ret==-1)
	{
		perror("ERROR listening to scoket:");
		exit(0);
	}

	//accept connection from scoket
	client_id =  accept(socket_id,(struct sockaddr *) NULL,NULL);
	if (client_id == -1)
	{
		perror("ERROR connecting to socket:");
		exit(0);
	}
	/***********************************************************************/
	/*********************************audio*********************************/
	const pa_sample_spec sample_spec = {
		.format = PA_SAMPLE_S16LE,
		.rate = 44100,
		.channels = 2
	};

	//create new connection
	// pa_simple_new(server,application name,stream direction,dev,stream_name(desc),sample_spec,channel_map,buffer_attr,error);
	play = pa_simple_new(NULL,"play_server.c",PA_STREAM_PLAYBACK,NULL,"play",&sample_spec,NULL,NULL,NULL);
	if (play ==NULL)
	{
		perror("ERROR connecting to audio device:");
		exit(0);
	}
	/***********************************************************************/
	
	char play_buffer[BUFF_SIZE],snd_buffer[100]=":)";
	while(1)
	{

		//recieve data from client
		ret = recv(client_id,play_buffer,BUFF_SIZE,0);
		if (ret ==-1)
		{
			//client_id =  accept(socket_id,(struct sockaddr *) NULL,NULL);
			//if (client_id == -1)
			//{
			//	perror("ERROR connecting to socket:");
			//	exit(0);
			//}
			perror("ERROR recieving data:");
			exit(0);
		}

		//play audio 
		ret = pa_simple_write(play,play_buffer,sizeof(play_buffer),NULL);
		if (ret ==-1)
		{
			perror("ERROR playing audio:");
			exit(0);;
		}

		//send acknwoledgemetn to client
		ret = send(client_id,snd_buffer,strlen(snd_buffer)+1,0);
		if (ret ==-1)
		{
			perror("ERROR sending to socket:");
			exit(0);
		}
	}
	//check all data is played 
	// ret = pa_simple_drain(play,NULL);
	// if (ret<0)
	// {
	// 	perror("ERROR drain:");
	// 	exit(0);
	// }

	close(socket_id);
	pa_simple_free(play);

	return 0;
}
