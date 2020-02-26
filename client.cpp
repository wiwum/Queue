/*
 * client.cpp
 * This code provides a test platform for LAnix IPC messaging implementation.
 * The client opens a message queue with a server application then sends messages
 * through the queue.  Each message is assigned a priority that must be handled
 * by the LAnix queues.
 * 
 * This is part of a series of labs for the Liberal Arts and Science Academy.
 * The series of labs provides a mockup of an POSIX Operating System
 * referred to as LA(SA)nix or LAnix.
 *  
 * (c) copyright 2018 by James Shockey
 * 
 * */
 
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>			//provides access to the POSIX operating system API
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <ctime>
//#include <mqueue.h>
#include <pthread.h>

#include "lqueue.h"

class thread_data {
	public:
		int  thread_id;
		char *message;
};

#define SERVER_QUEUE_NAME   "/LAQueue server"
#define QUEUE_PERMISSIONS 0660
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10

//int client_main (int argc, char **argv)
void *client_main (void *threadarg)
{
    char client_queue_name [64];
    lqd_t qd_server;
	lqd_t qd_client;   // queue descriptors
	


    // create the client queue for receiving messages from server
    sprintf (client_queue_name, "/q-client-%d", getpid ());
	perror ("Client: Hello World");

    struct lq_attr attr;

    attr.lq_flags = 0;
    attr.lq_maxmsg = MAX_MESSAGES;
    attr.lq_msgsize = MAX_MSG_SIZE;
    attr.lq_curmsgs = 0;

	// Create the client queue and connect to server queue
    if ((qd_client = lq_open (client_queue_name, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
        perror ("Client: lq_open error (client)");
        exit (1);
    }

    if ((qd_server = lq_open (SERVER_QUEUE_NAME, O_WRONLY)) == -1) {
        perror ("Client: lq_open error 2 args (server)");
        exit (1);
    }

    char in_buffer [MSG_BUFFER_SIZE];
    char out_buffer [MSG_BUFFER_SIZE];

	//Tell the server the name of the client message queue
	if (lq_send (qd_server, client_queue_name, strlen (client_queue_name), 0) == -1) {
		perror ("Client: Not able to send client info to server");
	}


	int i = 0;
	
    // Run the client for a period of time.
	double TimeToRunInSecs = 0.4;
	clock_t c = clock();
	while(double(clock()-c)/CLOCKS_PER_SEC < TimeToRunInSecs)
	{


		usleep(200);
		
		// Generate a series of buffers that contain lowercase letters
		//		and send the buffers to the server at differing priorities
		if (i < 26)
		{
			for (int j=0; j < 10; j++ )
				out_buffer[j] = (char)('a'+i);
			out_buffer[10] = 0;
			int priority = i % 4;

			if (lq_send (qd_server, out_buffer, strlen (out_buffer), priority) == -1) {
				perror ("Client: Not able to send message to server");
			}
		}
        // receive response from server
		int bufLen = 0;
        if ((bufLen = lq_receive (qd_client, in_buffer, 256, 0)) == -1) {
            perror ("Client: lq_receive error");
            exit (1);
        }
		in_buffer[bufLen] = 0; 

        // display token received from server
		if (bufLen > 0)
			printf ("Client: Token received from server: %s\n", in_buffer);
		
		i++;

    }
	// Wrap up the quewue
	if (lq_unlink (client_queue_name) == -1) {
        perror ("Client: lq_unlink - client");
        exit (1);
    }


	printf ("Client: bye\n");

	return 0;

}