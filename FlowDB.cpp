// FlowDB.cpp : Defines the entry point for the console application.
//

#include <iostream>
//#include <math.h>
#include "mem.h"


int main(int argc, char* argv[])
{
	/*BPTree *bptr = new BPTree();
	std::cout << "FlowDB v. 0.0.1" << std::endl;
	for(int i=1; i<10; i++) {
		bptr->Insert(i, i + 64);
	}
	std::cout << "Data: " << bptr->FindValue(10) << std::endl;
	//*/

	int *rand_ints;
	ManagedMemory *mem = new ManagedMemory();
	rand_ints = (int *)mem->OpenFile("rand_ints.fdb");
	//rand_ints = new int[100];

	for(int i=0; i<100; i++) rand_ints[i] = i;

	mem->CloseFile();
	//std::getchar();
	return 0;
}
