// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "elevatortest.h"

// testnum is set in main.cc
int testnum = 1;
#define Max_TreadNum 128

//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------

void
SimpleThread(int which)
{
    int num;
	currentThread->Yield();
    
	//printf("***  %d TheardID %d looped %d times \n", currentThread->getName(),currentThread->getThreadID(), num);

}

//----------------------------------------------------------------------
// ThreadTest1
// 	Set up a ping-pong between two threads, by forking a thread 
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void
ThreadTest1()
{
    DEBUG('t', "Entering ThreadTest1");

//    for(int i=0;i<50;i++){
//    	Thread *t = new Thread("my thread");
//    	printf("*** %s TheardID %d  UserID %d \n", t->getName(),t->getThreadID(), t->getUserID());
    	Thread *t1 = new Thread("my thread1" ,1);
    	t1->Fork(SimpleThread,(void *) 0);
    	Thread *t2 = new Thread("my thread1" ,3);
    	t2->Fork(SimpleThread,(void *) 0);
    	Thread *t3 = new Thread("my thread1" ,5);
    	t3->Fork(SimpleThread,(void *) 0);
    	Thread *t4 = new Thread("my thread1" ,4);
    	t4->Fork(SimpleThread,(void *) 0);
	currentThread->Yield();
//    }




}

//----------------------------------------------------------------------
// ThreadTest
// 	Invoke a test routine.
//----------------------------------------------------------------------

void
ThreadTest()
{
    switch (testnum) {
    case 1:
	ThreadTest1();
	break;
    default:
	printf("No test specified.\n");
	break;
    }
}
void
TS()
{
	DEBUG('t',"Entering TS");

	const char* ToString[]{"JUST_CREATED","RUNNING", "READY", "BLOCKED"};

	printf("---------------------Call Ts-----------------------\n");
	for(int i=0;i<128;i++){
		if(ThreadID_Mark[i]==1)
			printf("*** %s TheardID %d  UserID %d ThreadStatus %s ThreadPriority   \n", Threadpointer[i]->getName(),Threadpointer[i]->getThreadID(), Threadpointer[i]->getUserID(),ToString[Threadpointer[i]->getStatus()]);
	}
	printf("----------------------------------------------------\n");

}

