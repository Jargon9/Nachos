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
#include "synch.h"

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


//-------------------------------------------------------------------------
void
TS()
{
	DEBUG('t',"Entering TS");

	const char* ToString[]{"JUST_CREATED","RUNNING", "READY", "BLOCKED"};

	printf("---------------------Call Ts-----------------------\n");
	for(int i=0;i<128;i++){
		if(ThreadID_Mark[i]==1)
			printf("*** %s TheardID %d  UserID %d ThreadStatus %s \n", Threadpointer[i]->getName(),Threadpointer[i]->getThreadID(), Threadpointer[i]->getUserID(),ToString[Threadpointer[i]->getStatus()]);
	}
	printf("----------------------------------------------------\n");

}
//-------------------------------------------------------------------------

void
SimpleThread(int which)
{
    int num;
    
	printf("***  %d TheardID %d looped %d times \n", currentThread->getName(),currentThread->getThreadID(), num);

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

    for(int i=0;i<10;i++){
    	Thread *t = new Thread("my thread");
    	printf("*** %s TheardID %d  UserID %d \n", t->getName(),t->getThreadID(), t->getUserID());
    }
}

//----------------------------------------------------------------------
// ThreadTest
// 	Invoke a test routine.
//--------------------------条件变量实现--------------------------------------------
//Condition* Conprod = new Condition("Producer");
//Condition* Concons = new Condition("Consumer");
//Lock* lockpc = new Lock("lockprocon");
//
//static int keyvalue = 0;
//
//void
//Producer(int time){
////    TS();
//	int x=1;
//	printf("time: %d \n", time);
//	while((time--)>0){
////		printf("time: %d \n", time);
////		printf("produce%d keyAcquire \n",x);
//		lockpc->Acquire();
////		Conprod->Wait(lockpc);
////		printf("before produce%d keyValue:%d \n",x,keyvalue);
//		keyvalue++;
//		printf("produce%d worked keyValue:%d \n",x,keyvalue);
//		Concons->Signal(lockpc);
//		lockpc->Release();
//		DEBUG('t', "Thread：%s sleep",currentThread->getName());
//		Conprod->Wait(lockpc);
//	}
//}
//
//void
//Consumer(int time){
//	int x=2;
////    TS();
//	printf("time: %d \n", time);
//	while((time--)>0){
////		printf("time: %d \n", time);
//		lockpc->Acquire();
//		if(keyvalue<=0)
//			Concons->Wait(lockpc);
//		keyvalue--;
//		printf("Consumer%d worked keyValue:%d \n",x,keyvalue);
//		Conprod->Signal(lockpc);
//		lockpc->Release();
//		DEBUG('t', "Thread：%s sleep",currentThread->getName());
//		Concons->Wait(lockpc);
//	}
//}
//***************************信号量实现**********************
Semaphore* lock = new Semaphore("Lock", 1);
Semaphore* empty = new Semaphore("Empty", 10); //缓冲区为10
Semaphore* full = new Semaphore("Full", 0);
int applenum=0;
void
Producer(int time){
//	printf("time: %d \n", time);
//	while(true){
//		printf("time: %d \n", time);
//		printf("produce%d keyAcquire \n",x);
		empty->P();
		lock->P();
		applenum++;
		printf("%s puts a apple into buffer, apple num:%d  \n",currentThread->getName(),
				applenum);
		lock->V();
		full->V();

//		Conprod->Wait(lockpc);
//		printf("before produce%d keyValue:%d \n",x,keyvalue);
//	}
}

void
Consumer(int time){
	int x=2;
//    TS();
//	printf("time: %d \n", time);
//	while(true){
		full->P();
		lock->P();
		applenum--;
		printf("%s puts a apple into buffer, apple num:%d  \n",currentThread->getName(),
				applenum);
		lock->V();
		empty->V();

//	}
}

void
ThreadTest2()
{
    DEBUG('t', "Entering ThreadTest2");
    Thread* p1 = new Thread("Produce1");
    Thread* p2 = new Thread("Produce2");
    p1->Fork(Producer,5);
    p2->Fork(Producer,6);
    Thread* p5 = new Thread("Consumer1");
    Thread* p6 = new Thread("Consumer2");
    p5->Fork(Consumer,8);
    p6->Fork(Consumer,7);
    Thread* p3 = new Thread("Produce3");
    Thread* p4 = new Thread("Produce4");
    p3->Fork(Producer,8);
    p4->Fork(Producer,7);

    Thread* p7 = new Thread("Consumer3");
    Thread* p8 = new Thread("Consumer4");
    p7->Fork(Consumer,6);
    p8->Fork(Consumer,5);
}

//-----------------------------实现barrier-------------------------------------------
Semaphore* Threadlock[5];
Lock* barrierlock=new Lock("barrierlock");
Condition* barrier=new Condition("barrier");
int barriernum=0;

void InitThreadlock(int x){
	for(int i=0;i<x;i++){
		Threadlock[i]=new Semaphore("Lock", 1);
	}
}

void Threadbarrier(int x){

	for(int i=0;i<5;i++){
		barriernum++;
//		Threadlock[x]->P();
		printf("barriertest%d print %d \n", x, i+1);
//		Threadlock[x]->V();
		if(barriernum==5){
//			printf("barriertest%d new round %d \n", x);
			barrier->Broadcast(barrierlock);
			barriernum=0;
		}
		else{
		    DEBUG('t', "barrierlock wait \n");
			barrier->Wait(barrierlock);

		}
	}
}
void
ThreadTest3()
{
	InitThreadlock(5);
    DEBUG('t', "Entering ThreadTest3");
    Thread* p1 = new Thread("barriertest1");
    Thread* p2 = new Thread("barriertest2");
    Thread* p3 = new Thread("barriertest3");
    Thread* p4 = new Thread("barriertest4");
    Thread* p5 = new Thread("barriertest5");
    p1->Fork(Threadbarrier,1);
    p2->Fork(Threadbarrier,2);
    p3->Fork(Threadbarrier,3);
    p4->Fork(Threadbarrier,4);
    p5->Fork(Threadbarrier,5);

}
//-----------------------------------Reader and writer-----------------------------------------

Lock* lockrw = new Lock("lockrw");
Lock* lockr = new Lock("lockr");

int Secret=10,rc=0;



void
Reader(int time){
	while((time--)>0){
		lockr->Acquire();
		rc=rc+1;
		if(rc==1)lockrw->Acquire();
		lockr->Release();
		printf(" %s secret:%d  Readercount:%d\n",currentThread->getName(),Secret, rc);
		currentThread->Yield();
		lockr->Acquire();
		rc=rc-1;
		printf("Readercount decreased:%d \n",rc);
		if(rc==0)lockrw->Release();
		lockr->Release();
	}
}

void
Writer(int time){
	while((time--)>0){
		printf("Readercount :%d  \n",rc);
		lockrw->Acquire();
		printf("Writer changed the Secret:%d \n",Secret--);
		lockrw->Release();
	}
}
void
ThreadTest4()
{
	InitThreadlock(5);
    DEBUG('t', "Entering ThreadTest3");
    Thread* p1 = new Thread("Reader1");
    Thread* p2 = new Thread("Reader2");
    Thread* p3 = new Thread("Writer1");
    Thread* p4 = new Thread("Writer2");
    Thread* p5 = new Thread("Reader3");
    p1->Fork(Reader,1);
    p2->Fork(Reader,1);
    p3->Fork(Writer,1);
    p4->Fork(Writer,1);
    p5->Fork(Reader,1);

}




//--------------------------------------------------------------------------
void
ThreadTest()
{
    switch (4) {
    case 1:
//	ThreadTest2();
    case 2:
    	ThreadTest2(); //producer
    	break;
    case 3:
    	ThreadTest3();  //barrier
    	break;
    case 4:
    	ThreadTest4();
    	break;
    default:
	printf("No test specified.\n");
	break;
    }
}


