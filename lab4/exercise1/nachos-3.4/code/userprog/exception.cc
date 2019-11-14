// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "machine.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------
extern int TranslateCount,TlbMissCount;
int
	PageReplaceHander(int BadVAddr){
	int pageloc = -1,temp=-1;
	for(int i=0; i<machine->pageTableSize,i!=BadVAddr; i++){
		temp = i;
		if(machine->pageTable[i].dirty==FALSE){
			pageloc = i;
			DEBUG('a', "Find Nondirty page.\n");
			return i;
		}
	}
	if(pageloc == -1){
		OpenFile *VirMem = fileSystem->Open("VirtualMemory");
		VirMem->WriteAt(&(machine->mainMemory[pageloc * PageSize]), PageSize, machine->pageTable[pageloc].virtualPage*PageSize);
		pageloc = temp;//NOT FINISH TO Deal with dirty.
		delete VirMem;
	}
	machine->pageTable[pageloc].valid = FALSE;
	return pageloc;

}
void
	PageMISSHandler(int BadVAddr){
	int newPage = machine->alloFrame();
	if(newPage==-1){
		DEBUG('a', "AlloFrame fail.\n");
		newPage = PageReplaceHander(BadVAddr);
		DEBUG('a', "Replace AlloFrame %d.\n", newPage);
	}
	machine->pageTable[BadVAddr].physicalPage = newPage;
	OpenFile *VirMem = fileSystem->Open("VirtualMemory");

	VirMem->ReadAt(&(machine->mainMemory[newPage*PageSize]), PageSize, BadVAddr*PageSize);
	delete VirMem;

	machine->pageTable[BadVAddr].valid = TRUE;
	machine->pageTable[BadVAddr].use = FALSE;
	machine->pageTable[BadVAddr].dirty = FALSE;
	machine->pageTable[BadVAddr].readOnly = FALSE;
	DEBUG('a', "PageMIssHandler successful .\n");

}
int
	FIFO(){
	int pos=TLBSize - 1;
	for( int i=0; i< TLBSize -1;i++){
		machine->tlb[i] = machine->tlb[i+1];
	}
	return pos;
}
int
	LRU(){
	int pos=0,temp=machine->tlbtime[0];
	for( int i=0; i< TLBSize ;i++){
		if(machine->tlbtime[i]>temp){
			temp = machine->tlbtime[i];
			pos = i;
		}
	}
//	machine->tlbtime[pos]=0;
	return pos;
}
void
	TLBMISSHandler(int virAd){
	DEBUG('a', "TLB missHanding.\n");
	unsigned int vpn;
	vpn = (unsigned)virAd / PageSize;
	int pos=0;
	for(; pos< TLBSize ;pos++){

		if(machine->tlb[pos].valid == FALSE){
			printf("found place %d \n", pos);
			break;
		}

	}

	if(pos==4){
		pos = FIFO();
		DEBUG('a', "TLB missHanding FIFO pos:%d.\n",pos);
	}
	printf("Ready pagenum %d \n", pos);

	//this vpn means ppn
    for(int i=0;i<NumPhysPages;i++){
    	DEBUG('a', "pageTable[%d].threadID:%d , currentThread:%d valid:%d \n", i,
    			machine->pageTable[i].threadID, currentThread->getThreadID(),machine->pageTable[i].valid);
		if(machine->pageTable[i].valid==TRUE&& machine->pageTable[i].virtualPage==vpn && machine->pageTable[i].threadID==currentThread->getThreadID()){
			DEBUG('a', "pageTable[%d].valid==TRUE\n", vpn);
			machine->tlb[pos].valid = true;
			machine->tlb[pos].physicalPage = machine->pageTable[i].physicalPage;
		}
    }
	if(machine->tlb[pos].valid != true){
		DEBUG('a', "TLBMISS->PageMiss vpn:%d\n", vpn);
		ASSERT(FALSE);
//		PageMISSHandler(vpn);
//		machine->tlb[pos].physicalPage = machine->pageTable[vpn].physicalPage;
//		DEBUG('a', "TLBMISS->PageMiss success.\n");
	}


//	printf("pos %d \n", pos);

}


void
PrintTLBMiss(){
	double TlbMissRate = (double)TlbMissCount/(double)TranslateCount;
	double TlbHitRate = 100 - TlbMissRate *100;

	printf("TranslateCount:%d, TlbMissCount:%d, TlbHitRate:%.2lf%% \n",TranslateCount
			,TlbMissCount,TlbHitRate);
}


void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    if ((which == SyscallException) && (type == SC_Halt)) {
	DEBUG('a', "Shutdown, initiated by user program.\n");
		//PrintTLBMiss();
		machine->freeFrame();
		interrupt->Halt();
    }else if(which == PageFaultException){
    	DEBUG('a', "PageFault ExceptionHander.\n");
	   	int BadVAddr = machine->ReadRegister(BadVAddrReg);
	   	DEBUG('a', "get Virtualaddress.\n");
    	if(machine->tlb==NULL){
    		PageMISSHandler(BadVAddr);
    		printf("Unexpected user mode exception %s %s\n", which, type);
    		ASSERT(FALSE);
    	}
    	else{
    		DEBUG('a', "TLB miss.\n");
//    		printf("Unexpected user mode exception %s %s\n", "TLBMISS", type);
    	   	TLBMISSHandler(BadVAddr);
    	}

    }else {
	printf("Unexpected user mode exception %s %s\n", which, type);
	ASSERT(FALSE);
    }
}
