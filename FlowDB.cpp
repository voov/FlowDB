// FlowDB.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <math.h>
#include <sys/mman.h> // memory manager on POSIX

#define BUCKET_SIZE 3

class Bucket;
class Elem {
public:
	int key;
	char data;
	int revision=0;
	Bucket *childBucket;
	Bucket* infinityNode;

	int64_t address; // address of Elem in file

	Elem() {
		key = NULL;
		data = NULL;
		childBucket = NULL;
		infinityNode = NULL;
	}

	Elem(int p_key) {
		key = p_key;
		data = NULL;
		childBucket = NULL;
		infinityNode = NULL;
	}

	Elem(int p_key, char p_data) {
		key = p_key;
		data = p_data;
		childBucket = NULL;
		infinityNode = NULL;
	}

	void SetChildBucket(Bucket* p_child) {
		childBucket = p_child;
	}

	static Elem* Clone(Elem* e) {
		return new Elem(e->key, e->data);
	}

	Bucket* GetChildBucket() {
		return childBucket;
	}

	void SetInfinityNode(Bucket* infinity) {
		infinityNode = infinity;
	}

	Bucket* GetInfinityNode() {
		return infinityNode;
	}
};

class Bucket {
private:
	Bucket*	sibling;
	Bucket*	parentBucket;
	bool	isLeaf;
	Elem*	elements;
	int		elemCount;
public:
	Bucket() {
		sibling = NULL;
		//childBucket = NULL;
		parentBucket = NULL;
		isLeaf = true;
		elements = new Elem[BUCKET_SIZE];
		elemCount = 0;
	};

	~Bucket() {
		delete[] elements;
	};

	int Insert(int key, char data) {
		// Loop trough the bucket, and insert the elem in the appropriate position
		for(int i=0; i<elemCount;i++) {
			if(elements[i].key >= key) {
				// We should push elements right
				std::memmove(elements + i + 1, elements + i, sizeof(elemCount - i) * sizeof(Elem));
				elements[i].key = key;
				elements[i].data = data;
				elemCount++;

				// now fix the revision, if the key is the same

				return i;
			}
		}

		// We looped trough all, and didn't found anything
		elements[elemCount].key = key;
		elements[elemCount].data = data;
		elemCount++;

		//return elemCount-1;
		return elemCount-1;
	};

	void Remove(int index) {
		// We should pull elements to left
		//elements[index] = Elem(); // TODO: Ez ide minek? :s
		//std::memmove(elements + index, elements + index + 1, sizeof(elemCount - index - 1) * sizeof(Elem));
		//for(int i=index+1;i<elemCount;i++) {
		for(int i = index;i<elemCount;i++) {
			elements[i-1] = elements[i];
		}
		elemCount--;
	};

	int Size() {
		return elemCount;
	}

	bool IsLeaf() {
		return isLeaf;
	};

	void SetIsLeaf(bool leaf) {
		isLeaf = leaf;
	}

	bool IsFull() {
		// We need to watch out for one more elements, as it is the temporary holder
		if(elemCount >= BUCKET_SIZE - 1) return true;
		return false;
	}

	Elem* GetMedian() {
		int medianIdx = elemCount >> 1;
		return new Elem(elements[medianIdx].key, elements[medianIdx].data);
		//return &elements[medianIdx];
	};

	Elem* GetElem(int index) {
		return &elements[index];
	};

	Bucket* Split() {
		Bucket* buffer = new Bucket();
		int medianIdx = (elemCount >> 1) + 1;
		//int _count = elemCount;

		while(elemCount >= medianIdx) {
			// everything larger than median should go to new
			buffer->Insert(elements[medianIdx].key, elements[medianIdx].data);
			Remove(medianIdx); // TODO: REMOVES Median too, which was supposed to be cloned
		}

		return buffer;
	};

	Bucket* GetSibling() {
		return sibling;
	};

	void SetSibling(Bucket* p_sibling) {
		sibling = p_sibling;
	};

	void SetParentBucket(Bucket* p_root) {
		parentBucket = p_root;
	};

	Bucket* GetParentBucket() {
		return parentBucket;
	}


	void Dump() {
		std::cout << "-------------------" << std::endl;
		for(int i=0; i<elemCount; i++) {
			std::cout << "[" << i << ":" << elements[i].key << "=>" << elements[i].data  << "]";
		}
		std::cout << std::endl;
	}


};

class BPTree {
private:
	Bucket* rootBucket;
public:
	BPTree() {
		rootBucket = new Bucket();
	}

	~BPTree() {
		delete rootBucket;
	}

	char FindValue(int key) {
		Bucket *container = FindBucket(rootBucket, key);
		for(int i=0; i<BUCKET_SIZE;i++) {
			if(container->GetElem(i)->key == key) {
				return container->GetElem(i)->data;
			}

		}

		return NULL;
	}

	Bucket* FindBucket(Bucket* root, int key, bool foundKey=false) {
		// find the bucket where we can put the key into
		foundKey = false;
		while(!root->IsLeaf()) {
			root->Dump();
			for(int i=0; i<root->Size();i++) {
				Elem *curElem = root->GetElem(i);
				if(key == curElem->key) foundKey = true;

				// Go down the child node
				if(curElem->key >= key) {
					// follow child node
					root = curElem->childBucket;
					break;
				}

				// If no elements in the node are larger then the sought, and we are
				// at the end of the bucket, follow the infinity key
				// follow the infinity node when at the end of elemCount NOT BUCKET_SIZE
				if(curElem->key < key && i == root->Size()-1) {
					// follow the infinity
					root = curElem->infinityNode;
					break;
				}
			}
		}

		// We have found some leaf bucket where we should try to insert the key into
		return root;
	}

	void Insert(int key, char value) {
		std::cout << "Insert key: " << key << " value: " << value << std::endl;
		// We need to find the bucket we put the key into first
		Bucket *selBucket = FindBucket(rootBucket, key); // TODO: FindBucket
		if(selBucket->IsFull() == false) {
			// we simply push the elem
			selBucket->Insert(key, value);
		} else {
			// Get the median
			Elem* median = selBucket->GetMedian(); // TODO: Do we need to copy the median?

			//Elem* median = Elem::Clone(selBucket->GetMedian()); // Get median clone
			std::cout << "Bucket is full, median: " << median->key << std::endl;
			// Split the bucket
			Bucket* rightBucket = selBucket->Split();

			//if(selBucket->GetParentBucket() != NULL) {
				// There is a parent element
				// Push the median up the tree

				while(selBucket->GetParentBucket() != NULL) {
					Bucket *parent = selBucket->GetParentBucket();
					rightBucket->SetParentBucket(parent);
					if(parent->IsFull()) {
						// Insert elem
						parent->Insert(median->key, NULL);
						// Split the node
						median = selBucket->GetMedian(); // TODO: Memory HOG?
						rightBucket = parent->Split();
						std::cout << "Parent is full, splitting with median: " << median->key << std::endl;
						// Move up
						selBucket = parent;
					} else {
						// There is space left in the bucket
						int newIndex = parent->Insert(median->key, NULL);
						// Set the newly inserted's child node to the currently selected bucket
						parent->GetElem(newIndex)->childBucket = selBucket;
						parent->GetElem(newIndex)->infinityNode = rightBucket;
						std::cout << "Insert into parent (non full)" << std::endl;
						rightBucket->SetParentBucket(parent);
						break; // We've finished inserting the elem, and B+ structure is cool
					}
				}

				// TODO: What happens when we run out parent nodes?
			//} else {
				// No parent is found
				if(selBucket->GetParentBucket() == NULL) {
					// Create a new parent bucket
					Bucket *newParent = new Bucket();
					newParent->SetIsLeaf(false);
					// Push the median up
					newParent->Insert(median->key, NULL);
					std::cout << "Create new parent with elem: " << median->key << std::endl;
					// Set the pointers
					newParent->GetElem(0)->childBucket = selBucket;
					newParent->GetElem(0)->infinityNode = rightBucket;

					selBucket->SetParentBucket(newParent);
					rightBucket->SetParentBucket(newParent);

					std::cout << "L:";
					selBucket->Dump();
					std::cout << " R:";
					rightBucket->Dump();
					std::cout << std::endl;

					rootBucket = newParent;
				}
			//}
		}
	}
};


int main(int argc, char* argv[])
{
	BPTree *bptr = new BPTree();
	std::cout << "FlowDB v. 0.0.1" << std::endl;
	for(int i=1; i<10; i++) {
		bptr->Insert(i, i + 64);
	}
	std::cout << "Data: " << bptr->FindValue(10) << std::endl;
	//std::getchar();
	return 0;
}
