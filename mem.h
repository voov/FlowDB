/**
	FlowDB Memory Manager
*/

#ifndef F_MEM_H
#define F_MEM_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <iostream>

#ifdef WIN32
	#include "mmap.h" // mmap emulation layer on Windows
	#define int64_t unsigned long long // 64-bit variables should work on Windows too ;)
#else
	#include <sys/mman.h> // memory manager on POSIX
#endif

class ManagedMemory {
private:
	FILE* fileHandle;
	void AllocateSpace();
	int curSize;
	void* mem;
public:
	ManagedMemory();
	ManagedMemory(int p_minSize);
	~ManagedMemory();
	void* OpenFile(const char* filename);
	void CloseFile();
	void Resize(int newSize);
};

#endif
