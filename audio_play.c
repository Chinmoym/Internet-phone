/****************************************************************************************************
Author : Chinmoy Mohapatra (MT2016505)

Title:

To play audio from file using pulse
****************************************************************************************************/

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <pulse/simple.h>
#include <pulse/error.h>
#include <signal.h>

#define SIZE 100

pa_simple * play = NULL;

// void handler(int signo)
// {
// 	char response;
// 	if (signo==SIGINT)
// 	{
// 		printf("Do you want to stop?(Y/n)\n");
// 		scanf("%c",&response);
// 		if (response=='Y')
// 		{
// 			pa_simple_free(play);
// 			printf("Connection closed\n");
// 			exit(0);
// 		}
// 	}
// }

int main()
{

	// if (signal(SIGINT,handler)==SIG_ERR)
	// {
	// 	printf("Can't catch the signal\n");
	// }

	int ret;

	const pa_sample_spec sample_spec = {
		.format = PA_SAMPLE_S16LE,
		.rate = 44100,
		.channels = 2};

	//create new connection
	// pa_simple_new(server,application name,stream direction,dev,stream_name(desc),sample_spec,channel_map,buffer_attr,error);
	play = pa_simple_new(NULL,"Play.c",PA_STREAM_PLAYBACK,NULL,"play",&sample_spec,NULL,NULL,NULL);
	if (play == NULL)
	{
		perror("ERROR connection:");
		exit(0);
	}

	while (1)
	{
		unsigned int buffer[SIZE];

		//read data from file to buffer
		ret = read(STDIN_FILENO,buffer,sizeof(buffer));
		if(ret==-1)
		{
			perror("ERROR reading:");
			exit(0);
		}

		//play
		ret = pa_simple_write(play,buffer,sizeof(buffer),NULL);
		if (ret<0)
		{
			perror("ERROR write:");
			exit(0);
		}
	}

	//check all data is played 
	ret = pa_simple_drain(play,NULL);
	if (ret<0)
	{
		perror("ERROR drain:");
		exit(0);
	}


	//free or close the connection
	pa_simple_free(play);

	return 0;
}