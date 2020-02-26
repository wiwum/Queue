/*
 * LaQueue - sever.cpp 
 * A server for the LaQueue lab that opens a POSIX IPC message queue that accepts
 * text from a client, converts the text to upper case, and returns it to the client.
 * 
 * This is part of a series of labs for the Liberal Arts and Science Academy.
 * The series of labs provides a mockup of an POSIX Operating System
 * referred to as LA(SA)nix or LAnix.
 *  
 * (c) copyright 2018, James Shockey - all rights reserved
 * 
 * */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <fcntl.h>
#include <sys/stat.h>
//#include <mqueue.h>
#include <unistd.h>		//provides access to the POSIX operating system API
#include <time.h>
#include <ctime>
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

int letters[256];

//int server_main (int argc, char **argv)
void *server_main (void *threadarg)
{
	lqd_t qd_server = -1; 		// queue descriptors
	lqd_t qd_client = -1;		// queue descriptors
	
	char q_client_name[64];
    long token_number = 1; 	// next token to be given to client
	
	for (int i=0; i<255;i++)	// A diagnostic record if necessary
		letters[i] = 0;
	
    printf ("Server: Hello, World!\n");

    struct lq_attr attr;

    attr.lq_flags = 0;
    attr.lq_maxmsg = MAX_MESSAGES;
    attr.lq_msgsize = MAX_MSG_SIZE;
    attr.lq_curmsgs = 0;

    if ((qd_server = lq_open (SERVER_QUEUE_NAME, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
        perror ("Server: lq_open (server)");
        exit (1);
    }
    char in_buffer [MSG_BUFFER_SIZE+2];
    char out_buffer [MSG_BUFFER_SIZE];
	int outLen = 0;
	//char *pt_buffer;

	std::cout << "Sever message queue open."  << std::endl;


	double TimeToRunInSecs = 0.5;
	clock_t c = clock();
	while(double(clock()-c)/CLOCKS_PER_SEC < TimeToRunInSecs)
	{
		
		usleep(1000);			// put a little load on the system so our queue code can process things
		
        // get the oldest message with highest priority
        if ((outLen = lq_receive (qd_server, in_buffer, MSG_BUFFER_SIZE, NULL)) == -1) {
            perror ("Server: lq_receive");
        }

		//If there was data then process it.
		if (outLen > 0)
		{
			//Check for the client queue information
			if (in_buffer[0] == '/')
			{
				//Open the client queue for return messages
				strncpy(q_client_name, in_buffer,64);
				if ((qd_client = lq_open (q_client_name, O_WRONLY)) == -1) {
					perror ("Server: Not able to open client queue");
					continue;
				}
				continue;
			}
			
			// Convert the string into output buffer and return to client
			strncpy(out_buffer, in_buffer,64);
			letters[(int)(out_buffer[0])]++;
			for(int i=0; ((i < outLen) && (i < 64)); i++){
				out_buffer[i] = toupper(out_buffer[i]);
			}
		
			if (lq_send (qd_client, out_buffer, strlen (out_buffer), 0) == -1) {
				perror ("Server: Not able to send message to client");
				continue;
			}
			
		}
		
        //printf ("Server: response sent to client. %d. sq: %d. cq: %d.\n", token_number, qd_server, qd_client);
        token_number++;

    }		//end while loop
	printf ("Server: exit.\n");

	// Clean up our message queue links
	if (lq_unlink (q_client_name) == -1) {
        perror ("CServer: lq_unlink - client");
        exit (1);
    }

		
	if (lq_unlink (SERVER_QUEUE_NAME) == -1) {
        perror ("Server: lq_unlink - server");
        exit (1);
    }

  
	return 0;
}