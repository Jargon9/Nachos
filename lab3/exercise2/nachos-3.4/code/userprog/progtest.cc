// progtest.cc 
//	Test routines for demonstrating that Nachos can load
//	a user program and execute it.  
//
//	Also, routines for testing the Console hardware device.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "console.h"
#include "addrspace.h"
#include "synch.h"

//----------------------------------------------------------------------
// StartProcess
// 	Run a user program.  Open the executable, load it into
//	memory, and jump to it.
//----------------------------------------------------------------------
void ForkThread(){
	printf("start second thread\n");
	machine->Run();
}

Thread*
CreateThread(OpenFile *executable,int number){
	printf("start new thread number:%d \n",number);

	char Name[15];
	sprintf(Name, "Uprogram %d",number);
	Thread *thread = new Thread(strdup(Name));

	AddrSpace *space;
	space = new AddrSpace(executable);
	thread->space = space;
	return thread;
}
void
Userpro(int number){
	printf("Running uprogram thread %d \n", number);

    printf("currentName: %s \n",currentThread->getName());
	currentThread->space->InitRegisters();
	currentThread->space->RestoreState();

	machine->Run();
//	ASSERT(FALSE);
    currentThread->Yield();

}
void
StartProcess(char *filename)
{

    OpenFile *executable = fileSystem->Open(filename);


    if (executable == NULL) {
	printf("Unable to open file %s\n", filename);
	return;
    }


    Thread *thread1 = CreateThread(executable,1);
    Thread *thread2 = CreateThread(executable,2);

    delete executable;
    thread1->Fork(Userpro, (void*)1);
    thread2->Fork(Userpro, (void*)2);

    printf("currentName: %s \n",currentThread->getName());
    currentThread->Yield();
    printf("currentName: %s \n",currentThread->getName());

//    printf("first init \n");
//    space1 = new AddrSpace(executable1);
//    printf("Second init \n");
//    space2 = new AddrSpace(executable1);
//    currentThread->space = space1;
//    printf("currentID: %s \n",currentThread->getName());
//    space2->InitRegisters();		// set the initial register values
//    space2->RestoreState();
//    thread->space = space2;
//    thread->Fork(ForkThread , 1);
//    currentThread->Yield();
//    printf("currentID again: %d \n",currentThread->getThreadID());
//    delete executable1;			// close file
//    space1->InitRegisters();		// set the initial register values
//    space1->RestoreState();		// load page table register
//    printf("first start \n");
//    machine->Run();			// jump to the user progam
//    ASSERT(FALSE);			// machine->Run never returns;
//					// the address space exits
//					// by doing the syscall "exit"
}

// Data structures needed for the console test.  Threads making
// I/O requests wait on a Semaphore to delay until the I/O completes.

static Console *console;
static Semaphore *readAvail;
static Semaphore *writeDone;

//----------------------------------------------------------------------
// ConsoleInterruptHandlers
// 	Wake up the thread that requested the I/O.
//----------------------------------------------------------------------

static void ReadAvail(int arg) { readAvail->V(); }
static void WriteDone(int arg) { writeDone->V(); }

//----------------------------------------------------------------------
// ConsoleTest
// 	Test the console by echoing characters typed at the input onto
//	the output.  Stop when the user types a 'q'.
//----------------------------------------------------------------------

void 
ConsoleTest (char *in, char *out)
{
    char ch;

    console = new Console(in, out, ReadAvail, WriteDone, 0);
    readAvail = new Semaphore("read avail", 0);
    writeDone = new Semaphore("write done", 0);
    
    for (;;) {
	readAvail->P();		// wait for character to arrive
	ch = console->GetChar();
	console->PutChar(ch);	// echo it!
	writeDone->P() ;        // wait for write to finish
	if (ch == 'q') return;  // if q, quit
    }
}
