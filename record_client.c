/********************************************************************************************************************
Author: Chinmoy Mohapatra (MT2016505)

Title: 
This program is a client program to take input from microphone and send the recorded voice over socket to the server

compile: gcc record_client.c -o record_client -lpulse-simple
execute: ./record_client [IP] [PORT]

********************************************************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pulse/simple.h>
#include <pulse/error.h>


#define BUF_SIZE 1024

int ret,socket_id;

struct sockaddr_in server_address;

pa_simple * record = NULL;

//signal handler to handle exit of program
void handler(int signo)
{
	char response;
	if (signo==SIGINT)
	{
		// ask user whether to disconnect or not
		printf("Disconnect the client call? (Y/n)\n");
		scanf("%c",&response);
		if (response == 'Y' | response =='y')
		{
			//free or close the audio resource
			pa_simple_free(record);
			
			//free the socket resource
			ret = close(socket_id);
			if (ret==-1)
			{
				perror("Erorr closing socket force exit:");
				exit(0);
			}

			printf("Connection closed.\n");
			exit(0);
		}
	}
}

//create socket connection
void socket_create(char * IP, int PORT)
{
	socket_id = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (socket_id == -1)
	{
		perror("ERROR creating socket:");
		exit(0);
	}
	//assign port number and ip to the socket
	bzero(&server_address,sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);
	server_address.sin_addr.s_addr = inet_addr(IP);
	bzero(server_address.sin_zero,sizeof(server_address.sin_zero));
}

int main(int argc, char * argv[])
{
	//habndle ctrl + c while closing the connection
	if (signal(SIGINT,handler)==SIG_ERR)
	{
		printf("Can't catch the signal\n");
	}

	//check if all the parameter are provided or not
	if (argc < 3)
	{
		printf("Not enough parameter. Please provide IP address and Port number to connect.\nFormat ./record_client [IP] [PORT]\n");
		exit(0);
	}

	/**************************************socket*****************************************/ 
	//get the IP address and port number from command line 
	char * IP = argv[1];
	int PORT = atoi(argv[2]);

	// check the validity of port number.
	if (PORT < 1024 && PORT > 49151)
	{
		printf("The port no %d is reserved.\n",PORT);
		exit(0);
	}


	//create - conect - write - close <---- for socket
	//create socket connection 
	socket_create(IP,PORT);

	//connect to socket
	ret = connect(socket_id,(struct sockaddr *) &server_address,sizeof(server_address));
	if (ret == -1)
	{
		perror("ERROR connecting to socket:");
		exit(0);
	}
	/************************************************************************************/
	/***************************************audio****************************************/
	//specify the format,sampling rate and # channels to be used with audio device
	const pa_sample_spec sample_spec = {
		.format = PA_SAMPLE_S16LE,
		.rate = 44100,
		.channels = 2
	};

	//establish a connection with the audio device
	//pa_simple_new(server,application name,stream direction,dev,stream_name(desc),sample_spec,channel_map,buffer_attr,error);
	record = pa_simple_new(NULL,"record_client.c",PA_STREAM_RECORD,NULL,"recording",&sample_spec,NULL,NULL,NULL);
	if (record==NULL)
	{
		perror("ERROR connecting to audio device:");
		exit(0);
	}
	/************************************************************************************/
	
	char record_buffer[BUF_SIZE],recv_buffer[100];
	while(1)
	{
		//read from the audio device
		ret = pa_simple_read(record,record_buffer,sizeof(record_buffer),NULL);
		if (ret == -1)
		{
			perror("ERROR reading from microphone:");
			exit(0);
		}

		//send the message to the server
		ret = send(socket_id,record_buffer,BUF_SIZE,0);
		if (ret==-1)
		{
			perror("ERROR sending data to server:");
			exit(0);
		}

		//recieve acknowledgement from server
		ret = recv(socket_id,recv_buffer,100,0);
		if (ret ==-1)
		{
			perror("ERROR recieving server acknowledgement:");
			exit(0);
		}
	}

	close(socket_id);
	pa_simple_free(record);
	return 0;
}