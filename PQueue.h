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

struct node
{
	int priority;
	void* data;
	struct node *link;
};



class PQueue
{

	private:
		node *front;

	public:
	PQueue()
	{
		front = NULL;
	}
		
	void push(void *item, int priority);
	void* top();
	void pop();
	void display();


};
