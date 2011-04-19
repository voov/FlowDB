/**
	FlowDB Memory Manager
*/

#ifndef F_MEM_H
#define F_MEM_H

#pragma once
#pragma message("MEM_H")

#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>

#ifdef WIN32
	#include "mmap.h" // mmap emulation layer on Windows
	#define int64_t unsigned long long // 64-bit variables should work on Windows too ;)
#else
	#include <sys/mman.h> // memory manager on POSIX
#endif

class ManagedMemory {
private:
	int fileHandle;
	void AllocateSpace();
	int curSize;
	void* mem;
public:
	ManagedMemory();
	ManagedMemory(int p_minSize);
	~ManagedMemory();
	void OpenFile(const char* filename, void* memPart);
	void CloseFile();
	void Resize(int newSize);
};

#endif