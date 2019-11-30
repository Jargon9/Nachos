// filehdr.h 
//	Data structures for managing a disk file header.  
//
//	A file header describes where on disk to find the data in a file,
//	along with other information about the file (for instance, its
//	length, owner, etc.)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#ifndef FILEHDR_H
#define FILEHDR_H

#include "disk.h"
#include "bitmap.h"
#include "time.h"
#include "string"


#define NumDirect 	((SectorSize - 2 * sizeof(int)) / sizeof(int))
#define MaxFileSize 	(NumDirect-1) * SectorSize + 32 * SectorSize
#define LengthOfTime 20 // "/0"
#define LengthOfFiletype 4

// The following class defines the Nachos "file header" (in UNIX terms,  
// the "i-node"), describing where on disk to find all of the data in the file.
// The file header is organized as a simple table of pointers to
// data blocks. 
//
// The file header data structure can be stored in memory or on disk.
// When it is on disk, it is stored in a single sector -- this means
// that we assume the size of this data structure to be the same
// as one disk sector.  Without indirect addressing, this
// limits the maximum file length to just under 4K bytes.
//
// There is no constructor; rather the file header can be initialized
// by allocating blocks for the file (if it is a new file), or by
// reading it from disk.

class FileHeader {
  public:
    bool Allocate(BitMap *bitMap, int fileSize);// Initialize a file header, 
						//  including allocating space 
						//  on disk for the file data
    void Deallocate(BitMap *bitMap);  		// De-allocate this file's 
						//  data blocks

    void FetchFrom(int sectorNumber); 	// Initialize file header from disk
    void WriteBack(int sectorNumber); 	// Write modifications to file header
					//  back to disk

    int ByteToSector(int offset);	// Convert a byte offset into the file
					// to the disk sector containing
					// the byte

    int FileLength();			// Return the length of the file 
					// in bytes

    void Print();			// Print the contents of the file.

    void getNowTime(char *nowTime){
    	char Time[20] = {0};
    	time_t now;
    	struct tm* timenow;
    	time(&now);
    	timenow = localtime(&now);
    	strftime(Time, sizeof(Time), "%Y:%m:%d-%H:%M:%S", timenow);
    	strcpy(nowTime, Time);
    }

    void SetType(char* types){
    	strcmp(this->type, types); //copy
    }
    char* GetType(){
    	return this->type;
    }
    void SetTimeOfCreate(){
    	char time[LengthOfTime] = {0};
    	getNowTime(this->TimeOfCreate);
//    	strcmp(this->TimeOfCreate, time);
    	printf("Set the least create time :%s \n", TimeOfCreate);

    }
    char* GetTimeOfCreate(){
    	return this->TimeOfCreate;
    }
    void SetTimeOfLVisit(){
    	char time[LengthOfTime] = {0};
    	getNowTime(this->TimeOfLVisit);
//    	printf("Set the least visited time ::%s \n", time);
//    	strcmp(this->TimeOfLVisit, time);
    	printf("Set the least visited time ::%s \n", TimeOfLVisit);
    }
    char* GetTimeOfLVisit(){
    	return this->TimeOfLVisit;
    }
    void SetTimeOfLModify(){
    	char time[LengthOfTime] = {0};
    	getNowTime(this->TimeOfLModify);
//    	strcmp(this->TimeOfLModify, time);
    	printf("Set the least modify time : :%s \n", TimeOfLModify);
    }
    char* GetTimeOfLModify(){
    	return this->TimeOfLModify;
    }
    int sector;

  private:
    int numBytes;			// Number of bytes in the file
    int numSectors;			// Number of data sectors in the file
    int dataSectors[NumDirect];		// Disk sector numbers for each data 
					// block in the file
    char type[LengthOfFiletype];    //describe the type of file;
    char TimeOfCreate[LengthOfTime];
    char TimeOfLVisit[LengthOfTime];
    char TimeOfLModify[LengthOfTime];
};

#endif // FILEHDR_H
