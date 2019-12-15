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


//----------------------------lab6--------------------------------
void
FileSysHander(int type){
	if(type==SC_Create){
		int address = machine->ReadRegister(4);  //read start position
		//------getName
		int pos = 0;
		int Name;
		char name[9 + 1];  //last pos is \0
		while(Name != '\0'){
			machine->ReadMem(address + pos, 1 ,&Name);
			name[pos++] = (char)Name;
		}
		name[pos] = '\0';
		fileSystem->Create(name, 0);
		printf("Create successful \n");
	}else if(type == SC_Open){
		currentThread->SaveUserState();
		int address = machine->ReadRegister(4);
		//------getName
		int pos = 0;
		int Name;
		char name[9 + 1];  //last pos is \0
		while(Name != '\0'){
			machine->ReadMem(address + pos, 1 ,&Name);
			name[pos++] = char(Name);
		}
		name[pos] = '\0';
		OpenFile *openfile = fileSystem->Open(name);
		currentThread->RestoreUserState();
		machine->WriteRegister(2, (int)openfile);
		printf("OpenFileId: %d \n", openfile);
		printf("Open successful \n");
	}else if(type == SC_Write){
		int address = machine->ReadRegister(4);
		int size = machine->ReadRegister(5);
		OpenFile *openFile = (OpenFile *)machine->ReadRegister(6);
		printf("openfile:%d \n", openFile);
		char *Buffer = new char[size + 1];
		int value, pos=0;
		for(;pos<size;pos++){
			bool success = machine->ReadMem(address++, 1 ,&value);
//			printf("%c \n", char(value));
			if(!success){
				address--;
				pos--;
				continue;
			}
//			printf("pos %d \n", pos);
			Buffer[pos] = char(value);
		}
//		printf("last %d \n", pos);
		Buffer[pos] = '\0';
		int numBytes = openFile->Write(Buffer, size);
		delete Buffer;
		machine->WriteRegister(2, numBytes);
		printf("Write successful \n");
	}else if(type == SC_Read){
		int address = machine->ReadRegister(4);
		int size = machine->ReadRegister(5);
		OpenFile *openfile = (OpenFile *)machine->ReadRegister(6);
		printf("OpenFIle : %d \n", openfile);
		char *Buffer = new char[size];

		int numBytes = openfile->Read(Buffer, size);
		for(int i=0;i<numBytes;i++){
			bool success = machine->WriteMem(address+i, 1, (int)Buffer[i]);
		}
		machine->WriteRegister(2, numBytes);
		printf("Read successful \n");
	}else if(type ==SC_Close){
		OpenFile *openfile = (OpenFile *)machine->ReadRegister(6);
		delete openfile;
		printf("close successful \n");
	}
}

void UserProHander(int type){
	if(type == SC_Exit){
		int status = machine->ReadRegister(4);
		DEBUG('t', "Exit Status of %d is %d\n", currentThread->getName(), status);
		printf("Exit \n");
		// Exit need all child process die
		if(currentThread->getchild() == 0){
			currentThread->Finish();
		}else{
			currentThread->exitchild();
		}
	}else if(type == SC_Exec){
		int address = machine->ReadRegister(4);  //read start position
		//------getName
		int pos = 0;
		int Name;
		char name[9 + 1];  //last pos is \0
		while(Name != '\0'){
			machine->ReadMem(address + pos, 1 ,&Name);
			name[pos++] = (char)Name;
		}
		name[pos] = '\0';
		// open file to exec
		OpenFile *exec = fileSystem->Open(name);
		AddrSpace *space;

		if(exec == NULL){
			printf("Unable to open file \n");
			return;
		}else{
			machine->WriteRegister(2, (int)exec);
		}
		space = new AddrSpace(exec);
		currentThread->space = space;

		delete exec;
		printf("Exec successful \n");
		space->InitRegisters();
		space->RestoreState();

		machine->Run();
		ASSERT(FALSE);
	}else if(type == SC_Join){
			printf("Join Process \n");
			currentThread->SaveUserState();
			int spaceid = (int)machine->ReadRegister(4);
			while(machine->AddrSpaceid[spaceid]!= 0){
//				if()
				currentThread->Yield();
			}
			machine->WriteRegister(2, 0);
			printf("Join Process \n");
	}
//	machine->WriteRegister(PCReg, arg);
//	machine->WriteRegister(NextPCReg, arg+4);
}
void ForkFunc(int arg){
	currentThread->space->RestoreState();

	machine->Run();
}
void ThreadHander(int type){
	if(type == SC_Fork){
//		printf("Forking \n");
		currentThread->SaveUserState();
		int funcAddr = machine->ReadRegister(4);
		currentThread->setchild();
		Thread *thread = new Thread("Test thread");
		thread->space = currentThread->space;
		thread->Fork(ForkFunc, funcAddr);
		currentThread->RestoreUserState();
		printf("Forking successful \n");

	}else if(type == SC_Yield){

		currentThread->SaveUserState();
		currentThread->Yield();
		currentThread->RestoreUserState();
		printf("Yield successful \n");
	}
}

void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);
    if (which == SyscallException){
		if (type == SC_Halt) {
		DEBUG('a', "Shutdown, initiated by user program.\n");
		interrupt->Halt();
		} else if(type == SC_Exit || type == SC_Exec || type == SC_Join){
			UserProHander(type);
		}else if(type == SC_Create || type == SC_Open || type == SC_Write || type == SC_Read || type == SC_Close){
			FileSysHander(type);
		}else if(type == SC_Fork || type == SC_Yield){
			ThreadHander(type);
		}
		machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));

		machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
		machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg)+4);
		return ;
    }
    else{
	printf("Unexpected user mode exception %d %d\n", which, type);
	ASSERT(FALSE);
    }
}
