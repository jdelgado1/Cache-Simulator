/* Shawn Tripathy and Jose Delgado
 * CSF Assignment 3
 * Helper.cpp
 * Helper functions
 */
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdlib.h>
#include <bitset>
#include <map>
#include <stdbool.h>
#include <bitset>
#include "helper.h"

using std::cout;
using std::endl;
using std::ifstream;
using std::cerr;
using std::string;
using std::cin;
using std::stoi;
using std::vector;
using std::getline;
using std::array;
using std::bitset;
using std::map;
using std::stringstream;
using std::hex;

/*
 * Reads in a string in hexadecimal
 * and translates it into binary
 * and returns the result as a string
 *
 */
string convertHexToBinary(string address) {
	string binary = address;
	stringstream ss;
	ss << hex << binary;
	unsigned n;
	ss >> n;
	bitset<32> b(n);
	return b.to_string();
}

/*
 * Is sent the commandline arguments and 
 * performs the operations to ensure 
 * they are all valid arguments
 *
 */
int checkInputs(int numSets, int numBlocks, int numBytes, string allocate, string write) {
	if (!((numSets & (numSets - 1)) == 0)) {
		cerr << "Invalid number of sets" << endl;
		return 1;
	}
	if (!((numBlocks & (numBlocks - 1)) == 0)) {
		cerr << "Invalid number of sets" << endl;
		return 1;
	}
	if ((numBytes < 4) || (!((numBytes & (numBytes - 1)) == 0))) {
		cerr << "Invalid block size" << endl;  
		return 1; 
	}
	if ((allocate.compare("no-write-allocate") == 0) && (write.compare("write-back") == 0)) {
		cerr << "Invalid commands" << endl;
		return 1;
	}
	
	return 0;

}

/*
 * Creates and initializes the blocks of the cache
 * based on the number of sets and blocks per set
 * then it returns the cache it creates
 *
 */
vector< vector<block> > createSetAssociative(int numSets, int numBlocksPerSet) {
        vector< vector<block> > theCache(numSets);
   	
	for (int i = 0; i < numSets; i++) {
         	vector<block> blocks(numBlocksPerSet);
        	for (int j = 0; j < numBlocksPerSet; j++) {
            		block s;
            		s.valid = false;
            		s.index = -1;
            		s.tag = -1;

            		s.dirty = false;
            		s.usedAt = -1;

         	   	blocks.at(j) = s;
        	}
      		theCache.at(i) = blocks;
    	}
    	return theCache;
}

/*
 * Recieves the address as a string and returns
 * the index of the input
 *
 */
int getIndex(string address, int numTagBits, int numIndexBits) {
	int index = 0;
	if (numIndexBits == 0) {
		index = 0;
	} 
	else {
		string indexBits = address.substr(numTagBits, numIndexBits);
		index = stoi(indexBits, 0, 2);
	}
	return index;
}

/*
 * Recieves the address as a string and returns
 * the tag of the input
 *
 */
int getTag(string address, int numTagBits) {
	string tagBits = address.substr(0, numTagBits);
	return stoi(tagBits, 0, 2);	
}

/*
 * Here we check to see if we have the block for the tag
 * We check if it is valid and we increase the load hits
 * if we do find it and it is valid. Otherwise if it is 
 * uninitialized or invalid we have a miss.
 * We return a boolean if the block was found regardless
 * if it was valid or not
 *
 */
bool lUpdateAtIndex(vector<vector<block>>& theCache, int index, int tag, int numBlocksPerSet, int numBytesPerBlock, int& loadHits, int& loadMisses, int& numAccesses, int& cycles) {
	bool foundBlock = false;
	int i = 0;
	for (i = 0; i < numBlocksPerSet; i++) {
		//block has the value we want to read 
		if ((theCache.at(index).at(i).tag) == tag && theCache.at(index).at(i).valid) {
			foundBlock = true;
		        loadHits++;
		        theCache.at(index).at(i).usedAt = numAccesses;
		        cycles++;
		        break;
		}
		//block is empty so we can update it
		if (theCache.at(index).at(i).tag == -1 && !(theCache.at(index).at(i).valid)) {
		        foundBlock = true;
		        theCache.at(index).at(i).index = index;
		        theCache.at(index).at(i).tag = tag;
		        theCache.at(index).at(i).valid = true;
		        theCache.at(index).at(i).broughtInAt = numAccesses;
		        loadMisses++;
			cycles = cycles + (numBytesPerBlock * 25 + 1);
			break;
		}
	}
	return foundBlock;

}

/*
 * Evicts with lru eviction strategy
 * This updates the load misses and cycles as well
 * 
 */
void llruEvict(vector<vector<block>>& theCache, int index, int tag, int numBlocksPerSet, int numBytesPerBlock, int numAccesses, int& loadMisses, int& cycles) {
	int lowest = theCache.at(index).at(0).usedAt; //where the least recently used block is
	int lowestIndex = 0;
        for (int k = 0; k < numBlocksPerSet; k++) {
	   	if (lowest > theCache.at(index).at(k).usedAt) {
			lowest = theCache.at(index).at(k).usedAt;
			lowestIndex = k;
		}
	}

        theCache.at(index).at(lowestIndex).index = index;
        theCache.at(index).at(lowestIndex).tag = tag;
        theCache.at(index).at(lowestIndex).valid = true;
        theCache.at(index).at(lowestIndex).broughtInAt = numAccesses;
        loadMisses++;		
	
	cycles = cycles + (numBytesPerBlock * 25 + 1);
	
}

/*
 * Evicts using the fifo eviction strategy
 * it also updates load misses and cycles 
 * appropriately
 *
 */
void lfifoEvict(vector<vector<block>>& theCache, int index, int tag, int numBlocksPerSet, int numBytesPerBlock, int numAccesses, int& loadMisses, int& cycles) {
	int lowest = theCache.at(index).at(0).broughtInAt; //where least recently used block is
	int lowestIndex = 0;
	for (int k = 0; k < numBlocksPerSet; k++) {
		if (lowest > theCache.at(index).at(k).broughtInAt) {
			lowest = theCache.at(index).at(k).broughtInAt;
			lowestIndex = k;
		}
	}
	theCache.at(index).at(lowestIndex).index = index;
	theCache.at(index).at(lowestIndex).tag = tag; 
	theCache.at(index).at(lowestIndex).valid = true;
	theCache.at(index).at(lowestIndex).broughtInAt = numAccesses;

	loadMisses++;

	cycles = cycles + (numBytesPerBlock * 25 + 1);
	
}

/*
 * Looks for the block during a store
 * and has different behaviors based on
 * different write behaviors and changes
 * cycles as well as store hits appropriately
 * A boolean is returned indicating if the block
 * was found
 *
 */
bool sUpdateAtIndex(vector<vector<block>>& theCache, int index, int tag, int numBlocksPerSet, int numAccesses, int& storeHits, int& cycles, string write) {
	bool found = false;
	for (int i = 0; i < numBlocksPerSet; i++) {
		//block has the value we want to read
		if (theCache.at(index).at(i).tag == tag && theCache.at(index).at(i).valid) {
			storeHits++;
			found = true;
			theCache.at(index).at(i).usedAt = numAccesses;
			if (write == "write-through") {
				theCache.at(index).at(i).index = index;
				theCache.at(index).at(i).tag = tag;
				theCache.at(index).at(i).valid = true;

				cycles++;
				cycles = cycles + 100 + 1;
			}
			else if (write == "write-back") {
				theCache.at(index).at(i).index = index;
				theCache.at(index).at(i).tag = tag;
				theCache.at(index).at(i).valid = true;
				cycles++;
				theCache.at(index).at(i).dirty = true;
			}
			break;
		}
	}
	return found;
}

/*
 * If the block was not found earlier, this 
 * proceeds to allocate based on the indicated
 * allocation behavior while also updating
 * store misses and cycles appropriately
 * A boolean is returned indicating if we 
 * found an invalid unused tag space to store in
 *
 */
bool doAllocate(vector<vector<block>>& theCache, int index, int tag, int numBlocksPerSet, int numBytesPerBlock, int numAccesses, int& storeMisses, int& cycles, string allocate) {
	bool found = false;
	for (int i = 0; i < numBlocksPerSet; i++) {
		//block is empty so we can update it
		if (theCache.at(index).at(i).tag == -1 && !theCache.at(index).at(i).valid) {
			storeMisses++;
			found = true;
			if (allocate == "write-allocate") {
				theCache.at(index).at(i).index = index;
				theCache.at(index).at(i).tag = tag;
				theCache.at(index).at(i).valid = true;
				theCache.at(index).at(i).broughtInAt = numAccesses;
				cycles = cycles + (numBytesPerBlock * 25 + 1);
			}
			else if (allocate == "no-write-allocate") {
				cycles = cycles + 100 + 1;
			}
			break;
		}
	}
	return found;
}

/*
 * Evicts according to lru eviction while
 * also increasing the store misses and cycles
 * appropriately
 *
 */
void slruEvict(vector<vector<block>>& theCache, int index, int tag, int numBlocksPerSet, int numBytesPerBlock, int numAccesses, int& storeMisses, int& cycles, string allocate) {
	int lowest = theCache.at(index).at(0).usedAt; //where the least recently used block is
		int lowestIndex = 0;
		for (int k = 0; k < numBlocksPerSet; k++) {
			if (lowest > theCache.at(index).at(k).usedAt) {
				lowest = theCache.at(index).at(k).usedAt;
				lowestIndex = k;
			}
		}

		storeMisses++;
		//found = true;
		if (allocate == "write-allocate") {
			theCache.at(index).at(lowestIndex).index = index;
			theCache.at(index).at(lowestIndex).tag = tag;
			theCache.at(index).at(lowestIndex).valid = true;
			theCache.at(index).at(lowestIndex).broughtInAt = numAccesses;
		       	if (theCache.at(index).at(lowestIndex).dirty == true) { 
			 	cycles = cycles + (numBytesPerBlock * 25 + 1);			
		      	}					
			cycles = cycles + (numBytesPerBlock * 25 + 1);
		}
		else if (allocate == "no-write-allocate") {
			cycles = cycles + 100 + 1;
		}

}

/*
 * Evicts based on the fifo eviction method
 * while also increasing store misses and cycles
 * appropriately
 *
 */
void sfifoEvict(vector<vector<block>>& theCache, int index, int tag, int numBlocksPerSet, int numBytesPerBlock, int numAccesses, int& storeMisses, int& cycles, string allocate) {
	int lowest = theCache.at(index).at(0).broughtInAt; //where the least recently used block is
	int lowestIndex = 0;
	for (int k = 0; k < numBlocksPerSet; k++) {
		if (lowest > theCache.at(index).at(k).broughtInAt) {
			lowest = theCache.at(index).at(k).broughtInAt;
			lowestIndex = k;
		}
	}
				
	storeMisses++;
	//found = true;
	if (allocate == "write-allocate") {
		theCache.at(index).at(lowestIndex).index = index;
		theCache.at(index).at(lowestIndex).tag = tag;
	 	theCache.at(index).at(lowestIndex).valid = true;
	 	theCache.at(index).at(lowestIndex).broughtInAt = numAccesses;
		if (theCache.at(index).at(lowestIndex).dirty == true) { 
	   		cycles = cycles + (numBytesPerBlock * 25 + 1);			
		}					
	 	cycles = cycles + (numBytesPerBlock * 25 + 1);
	}
	else if (allocate == "no-write-allocate") {
		cycles = cycles + 100 + 1;
	}

}

