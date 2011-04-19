/**
	FlowDB Memory Manager
*/
#include "mem.h"

ManagedMemory::ManagedMemory() {
	curSize = 64 * 1024 * 1024; // start with 64MB
}

ManagedMemory::ManagedMemory(int p_minSize) {
	curSize = p_minSize;
}

ManagedMemory::~ManagedMemory() {
	CloseFile();
}

void ManagedMemory::CloseFile() {
	munmap(mem, curSize);
	close(fileHandle);
}

void ManagedMemory::AllocateSpace() {
	if(!fileHandle) exit(-1);
	lseek(fileHandle, curSize-1, SEEK_SET);
	write(fileHandle, '\0', 1);
}

void ManagedMemory::OpenFile(const char* filename, void* memPart) {
	fileHandle = open(filename, O_RDWR);
	if(!fileHandle) {
		//TODO: log error
		exit(-1);
	}
	// if file doesn't exist, allocate space
	struct stat buffer;
	int status;
	status = fstat(fileHandle, &buffer);
	if(buffer.st_size == 0) {
		AllocateSpace();
	} else {
		curSize = buffer.st_size;
	}

	memPart = mmap(0, curSize, PROT_READ | PROT_WRITE, MAP_SHARED, fileHandle, 0);
	mem = memPart;
}