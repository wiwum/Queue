/*
 * LaQueue
 * 
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
#include <mutex>

#include "lqueue.h"
#include "PQueue.h"


typedef int lqd_t;

#define LQUEUE_TABLE_MAX 10
struct lqueue_table_entry
{
	char	lq_name[64];		// Queue name
	int		refCount;			// How many references are outstanding.
	PQueue* lq_ptr;				// Pointer to the queue object
};

lqueue_table_entry	lqueue_table[LQUEUE_TABLE_MAX];
int lqueue_table_cur = 0;

// mutex to lock critical region
std::mutex mylock;

lqd_t lq_open (const char *__name, int __oflag)
{
	for (int i=0; i < LQUEUE_TABLE_MAX; i++)
		if(strcmp(lqueue_table[i].lq_name, __name) == 0)
			return i;
	return -1;
}
lqd_t lq_open (const char *__name, int __oflag, int mode, lq_attr *attr)
{
	if (__oflag & O_CREAT)
	{
		// On first create, clear all the structures
		if (lqueue_table_cur >= LQUEUE_TABLE_MAX)
			return -1;

		mylock.lock();

		if (lqueue_table_cur == 0)
		{
			for (int i=0; i < LQUEUE_TABLE_MAX-1; i++)
				lqueue_table[i].lq_name[0] = 0;
				
		}
		int i = 0;
		for (i=0; i < LQUEUE_TABLE_MAX; i++)
			if(strcmp(lqueue_table[i].lq_name, __name) == 0)
			{
				lqueue_table[i].refCount += 1;
				mylock.unlock();
				return i;
			}
		strcpy(lqueue_table[lqueue_table_cur].lq_name, __name);
		lqueue_table[lqueue_table_cur].refCount +=1;
		i = lqueue_table_cur;
		lqueue_table[i].lq_ptr = new PQueue();
		lqueue_table_cur += 1;
		mylock.unlock();
		return i;
	}
	mylock.unlock();
	return -1;
}
int lq_close (lqd_t __msgid)
{
	int i = __msgid;
	if(lqueue_table[i].lq_name[0] != 0)
	{
		mylock.lock();
		lqueue_table[i].refCount -= 1;
		if (lqueue_table[i].refCount == 0)
		{
			lqueue_table[i].lq_name[0] = 0;
			delete lqueue_table[i].lq_ptr;
		}
		mylock.unlock();
		return 0;
	}
	return 0;
}
int lq_send (lqd_t __msgid, const char *__msg, size_t __msg_len, unsigned int __msg_prio)
{
	mylock.lock();
	if(lqueue_table[__msgid].lq_name[0] == 0) {								// if msg_id is invalid, signal an error
		std::cout << "ls_send: non-queue" << std::endl;
		return -1;

	}

	char* local_buf = new char[__msg_len];
	memcpy(local_buf, __msg, __msg_len);
	local_buf[__msg_len] = 0;

	lqueue_table[__msgid].lq_ptr->push ((void *)local_buf, __msg_prio);
	
	mylock.unlock();
	return 0;
}
ssize_t lq_receive (lqd_t __msgid, char *__msg, size_t __msg_len, unsigned int *__msg_prio)
{
	ssize_t len = 0;
	
	if(lqueue_table[__msgid].lq_name[0] == 0)					// if msg_id is invalid, signal an error
		return -1;

	mylock.lock();

	char* local_buf = (char *)lqueue_table[__msgid].lq_ptr->top();
	if (local_buf == NULL)
	{
		mylock.unlock();		
		return len;
	} 
	len = strlen(local_buf);
	lqueue_table[__msgid].lq_ptr->pop();
	if(__msg_len < (unsigned)len) {				// if msg_id is invalid, signal an error
		mylock.unlock();		
		return -1;		
	}
	memcpy(__msg, local_buf,  len);
	__msg[len] = 0;

	mylock.unlock();
	return len;
}
int lq_notify (lqd_t __msgid, const struct sigevent *__notification)
{
	return 0;
}
int lq_unlink (const char *__name)
{
	for (int i=0; i < LQUEUE_TABLE_MAX; i++)
		if(strcmp(lqueue_table[i].lq_name, __name) == 0)
		{
			mylock.lock();
			lqueue_table[i].refCount -= 1;
			if (lqueue_table[i].refCount == 0)
			{
				lqueue_table[i].lq_name[0] = 0;
				delete lqueue_table[i].lq_ptr;
			}
			mylock.unlock();
			return 0;
		}
	return 0;
}
int lq_getattr (lqd_t __msgid, struct lq_attr *__mqstat)
{
	for (int i=0;i<LQUEUE_TABLE_MAX; i++)
		if (lqueue_table[i].lq_ptr != NULL)
			std::cout << i << ". " << lqueue_table[i].lq_name << std::endl;
	return 0;
}
int lq_setattr (lqd_t __msgid, const struct lq_attr *__mqstat, struct lq_attr *__omqattr)
{
	return 0;
}

