// synch.cc 
//	Routines for synchronizing threads.  Three kinds of
//	synchronization routines are defined here: semaphores, locks 
//   	and condition variables (the implementation of the last two
//	are left to the reader).
//
// Any implementation of a synchronization routine needs some
// primitive atomic operation.  We assume Nachos is running on
// a uniprocessor, and thus atomicity can be provided by
// turning off interrupts.  While interrupts are disabled, no
// context switch can occur, and thus the current thread is guaranteed
// to hold the CPU throughout, until interrupts are reenabled.
//
// Because some of these routines might be called with interrupts
// already disabled (Semaphore::V for one), instead of turning
// on interrupts at the end of the atomic operation, we always simply
// re-set the interrupt state back to its original value (whether
// that be disabled or enabled).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synch.h"
#include "system.h"

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	Initialize a semaphore, so that it can be used for synchronization.
//
//	"debugName" is an arbitrary name, useful for debugging.
//	"initialValue" is the initial value of the semaphore.
//----------------------------------------------------------------------

Semaphore::Semaphore(char* debugName, int initialValue)
{
    name = debugName;
    value = initialValue;
    queue = new List;
}

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	De-allocate semaphore, when no longer needed.  Assume no one
//	is still waiting on the semaphore!
//----------------------------------------------------------------------

Semaphore::~Semaphore()
{
    delete queue;
}

//----------------------------------------------------------------------
// Semaphore::P
// 	Wait until semaphore value > 0, then decrement.  Checking the
//	value and decrementing must be done atomically, so we
//	need to disable interrupts before checking the value.
//
//	Note that Thread::Sleep assumes that interrupts are disabled
//	when it is called.
//----------------------------------------------------------------------

void
Semaphore::P()
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts
    
    while (value == 0) { 			// semaphore not available
    printf("%s get lock fail \n",currentThread->getName());
	queue->Append((void *)currentThread);	// so go to sleep
	currentThread->Sleep();
    } 
    value--; 					// semaphore available, 
						// consume its value
    DEBUG('a',"currentThread %s P() value:%d \n",currentThread->getName(),value);
    
    (void) interrupt->SetLevel(oldLevel);	// re-enable interrupts
}

//----------------------------------------------------------------------
// Semaphore::V
// 	Increment semaphore value, waking up a waiter if necessary.
//	As with P(), this operation must be atomic, so we need to disable
//	interrupts.  Scheduler::ReadyToRun() assumes that threads
//	are disabled when it is called.
//----------------------------------------------------------------------

void
Semaphore::V()
{
    Thread *thread=NULL;

    DEBUG('a',"currentThread %s V() \n",currentThread->getName());
    IntStatus oldLevel = interrupt->SetLevel(IntOff);
    if(!queue->IsEmpty())
    	thread = (Thread *)queue->Remove();
    if (thread != NULL)	   // make thread ready, consuming the V immediately
    	scheduler->ReadyToRun(thread);
    value++;
    (void) interrupt->SetLevel(oldLevel);
}

// Dummy functions -- so we can compile our later assignments 
// Note -- without a correct implementation of Condition::Wait(), 
// the test case in the network assignment won't work!
Lock::Lock(char* debugName) {
	name = debugName;
	semaphore = new Semaphore("Lock", 1);
}
Lock::~Lock() {
	delete semaphore;

}
void Lock::Acquire() {
	DEBUG('a',"currentThread Accquire \n");
	IntStatus oldLevel = interrupt->SetLevel(IntOff); //disable interrupt
//	DEBUG('a',"currentThread Accquire \n");
	DEBUG('a',"currentThread %s Accquire \n",currentThread->getName());

	semaphore->P();
	OwerThread = currentThread;
    (void) interrupt->SetLevel(oldLevel);

}
void Lock::Release() {
	IntStatus oldLevel = interrupt->SetLevel(IntOff); //disable interrupt
//	DEBUG('a',"currentThread Release \n");
	DEBUG('a',"currentThread %s Release \n",currentThread->getName());
//	printf('a',"currentThread %s Release \n",currentThread->getName());

	OwerThread = NULL;
	semaphore->V();
    (void) interrupt->SetLevel(oldLevel);
}
bool Lock::isHeldByCurrentThread(){
	return OwerThread==currentThread;
}

Condition::Condition(char* debugName) {
	name=debugName;
	Threadqueue=new List();

}
Condition::~Condition() {
	delete Threadqueue;
}
void Condition::Wait(Lock* conditionLock) {
	IntStatus oldLevel = interrupt->SetLevel(IntOff); //disable interrupt
	conditionLock->Release();
	DEBUG('a',"Thread:%s Release \n",currentThread->getName());

	Threadqueue->Append(currentThread);
	DEBUG('a',"Thread:%s sleep \n",currentThread->getName());
	currentThread->Sleep();

	DEBUG('a',"Thread awake by signal \n");
	conditionLock->Acquire();
    (void) interrupt->SetLevel(oldLevel);
}
void Condition::Signal(Lock* conditionLock) {
	IntStatus oldLevel = interrupt->SetLevel(IntOff); //disable interrupt
	if(!Threadqueue->IsEmpty()){
		Thread* awakeThread=(Thread*)Threadqueue->Remove();
		scheduler->ReadyToRun(awakeThread);
	}

	DEBUG('a',"signal work \n");
    (void) interrupt->SetLevel(oldLevel);
}

//--------------awake all Thread----------------------
void Condition::Broadcast(Lock* conditionLock) {
	IntStatus oldLevel = interrupt->SetLevel(IntOff); //disable interrupt
	while(!Threadqueue->IsEmpty()){
		Thread* awakeThread=(Thread*)Threadqueue->Remove();
		scheduler->ReadyToRun(awakeThread);
	}

	DEBUG('a',"Broadcast work \n");
    (void) interrupt->SetLevel(oldLevel);
}
