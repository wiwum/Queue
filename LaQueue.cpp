/*
 * LaQueue
 * 
 * This is the top level code for the LaQueue lab.  This code launches
 * a server and a client that simulate two processes communicating
 * through the LAnix IPC messaging queues.  It sequences the launches
 * and includes a pause to allow the server to complete its configuration
 * before the client is launched.
 * 
 * This is part of a series of labs for the Liberal Arts and Science Academy.
 * The series of labs provides a mockup of an POSIX Operating System
 * referred to as LA(SA)nix or LAnix.
 *  
 * (c) copyright 2018, James Shockey - all rights reserved
 * 
 * */


#include <iostream>
#include <cstdlib>
#include <pthread.h>		//Let our code know we will link in the thread library
#include "client.h"
#include "server.h"

#include <unistd.h>		//provides access to the POSIX operating system API
#include <time.h>

#include "lqueue.h"		// The LAnix IPC message queue library

using namespace std;

#define NUM_THREADS 5

class thread_data {
	public:
		int  thread_id;
		char *message;
};


int main () {
	//Array that will hold thread id's created by the system
   pthread_t threads[NUM_THREADS];
   //information to be passed into the thread when created.
   thread_data td[NUM_THREADS];
   int rc;
   int i=0;

	td[0].thread_id = 0;
	td[0].message = (char *)"Server thread\n";
	td[1].thread_id = 1;
	td[1].message = (char *)"Client thread\n";
	
	/*
	 * int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void*), void *arg);
	 * pthread_t *thread: location for system to put information about the thread when created.
	 * const pthread_attr_t *attr: Attributes passwd to system describing the requested thread configuration.
	 * void *(*start_routine)(void*):  The routine within the thread to begin execution
	 * void *arg: The arguments passed into the thread, essentially the argc, argv of 'main(argc, argv)'
	 * */

	// Launcht the server thread
	rc = pthread_create(&threads[i], NULL, server_main, (void *)&td[0]);
	if (rc) {
		cout << "Error:unable to create server thread," << rc << endl;
		exit(-1);
	}
	
	usleep(5000);	// A chunk of time to allow the server to initalize itself
	
	// Launch the client thread
	rc = pthread_create(&threads[i], NULL, client_main, (void *)&td[1]);
	if (rc) {
		cout << "Error:unable to create client thread," << rc << endl;
		exit(-1);
	}

   pthread_exit(NULL);
   
}