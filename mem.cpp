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
	fclose(fileHandle);
}

void ManagedMemory::AllocateSpace() {
	if(!fileHandle) exit(-1);
	fseek(fileHandle, curSize-1, SEEK_SET);
	fputc('\0', fileHandle);
	fseek(fileHandle, 0, SEEK_SET);
}

void* ManagedMemory::OpenFile(const char* filename) {

	std::cout << "Open File" << std::endl;
	struct stat buffer;
	int status;
	status = stat(filename, &buffer);


	if(buffer.st_size == 0) {
		// File not yet created
		std::cout << "No File" << std::endl;
		fileHandle = fopen(filename, "w");
		AllocateSpace();
	} else {
		std::cout << "File: " << buffer.st_size << std::endl;
		// File already created
		curSize = buffer.st_size;
		fileHandle = fopen(filename, "r+");
	}

	if(!fileHandle) {
		//TODO: log error
		exit(-1);
	}

	int fno = fileno(fileHandle);
	mem = mmap(0, curSize, PROT_READ | PROT_WRITE, MAP_SHARED, fno, 0);
	return mem;
}
