/*****************************************************************************************************************
Author: Chinmoy Mohapatra (MT2016505)

Title:

Implement a simple audio recording using pulse 
*****************************************************************************************************************/

// #include <config.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <pulse/simple.h>
#include <pulse/error.h>
#include <signal.h>

#define SIZE 100

pa_simple * record = NULL;

// void handler(int signo)
// {
// 	char response;
// 	if (signo==SIGINT)
// 	{
// 		printf("Do you want to stop?(Y/n)\n");
// 		scanf("%c",&response);
// 		if (response=='Y')
// 		{
// 			pa_simple_free(record);
// 			printf("Connection closed\n");
// 			exit(0);
// 		}
// 	}
// }

int main()
{

	// if(signal(SIGINT,handler)==SIG_ERR)
	// {
	// 	printf("Can't catch the signal\n");
	// }


	int ret;

	const pa_sample_spec sample_spec = {
		.format = PA_SAMPLE_S16LE,
		.rate = 44100,
		.channels = 2};

	printf("Start speaking\n");

	//create a new connection
	// pa_simple_new(server,application name,stream direction,dev,stream_name(desc),sample_spec,channel_map,buffer_attr,error);
	record = pa_simple_new(NULL,"record.c",PA_STREAM_RECORD,NULL,"recording",&sample_spec,NULL,NULL,NULL);
	if (record == NULL)//check for successful connection
	{
		perror("ERROR connection:");
		exit(0);
	}

	while(1)
	{
		unsigned int buffer[SIZE];
		//record audio
		// pa_simple_read(pa_simple,data buffer,# bytes,error)
		ret = pa_simple_read(record,buffer,sizeof(buffer),NULL);
		if (ret < 0)//check successful read or record
		{
			perror("ERROR read:");
			exit(0);
		}

		//redirect the output to speaker (attach to STDOUT) this need to go to socket for transmission
		//ret = write(STDOUT_FILENO,buffer,sizeof(buffer));
		//if (ret==-1)
		//{
		//	perror("ERROR writing:");
		//	exit(0);
		//}
	}

	//free or close the device
	pa_simple_free(record);


	return 0;
}
