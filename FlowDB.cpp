// FlowDB.cpp : Defines the entry point for the console application.
//

#include <iostream>
//#include <math.h>
#include "btree.h";
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

	BPTree *tree;
	ManagedMemory *mem = new ManagedMemory();
	tree = (BPTree *)mem->OpenFile("rand_ints.fdb");
	if(!tree) tree = new BPTree();
	for(int i=1; i<10; i++) {
		tree->Insert(i, i + 64);
	}


	mem->CloseFile();
	delete mem;
	delete tree;
	//std::getchar();
	return 0;
}
