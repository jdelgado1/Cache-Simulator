/* Shawn Tripathy and Jose Delgado
 * CSF Assignment 3
 * csim.cpp
 * main function
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

int main(int argc, char* argv[]){	
	int loadHits = 0;
	int loadMisses = 0;
	int storeHits = 0;
     	int storeMisses = 0;
	int cycles = 0;
	    
	string input;
	string load = "l"; 
      	string store = "s";
	int numLoads = 0;
	int numStores = 0;
	int numAccesses = 0;

	int numSets = stoi (argv[1]);
	int numBlocksPerSet = stoi (argv[2]);
	int numBytesPerBlock = stoi (argv[3]);

	string allocate = argv[4];
	string write = argv[5];
	string evictStrat = argv[6];

	if (argc < 7) {
		return 1;
	}

	if (checkInputs(numSets, numBlocksPerSet, numBytesPerBlock, allocate, write) == 1) {
		return 1;
	}	

	int numIndexBits = log2(numSets);
	int numOffsetBits = log2(numBytesPerBlock);
	int numTagBits = 32 - numIndexBits - numOffsetBits;

	vector< vector<block> > theCache = createSetAssociative(numSets, numBlocksPerSet);

	while (getline(cin, input)) {
		string command = input.substr(0, 1);
		string address = input.substr(2, 10);

		address = convertHexToBinary(address);

		int index = getIndex(address, numTagBits, numIndexBits);
		int tag = getTag(address, numTagBits);

		if (command == "l") {
			bool foundBlock = lUpdateAtIndex(theCache, index, tag, numBlocksPerSet, numBytesPerBlock, loadHits, loadMisses, numAccesses, cycles);
			if (!foundBlock) {
				if (evictStrat == "lru") {
					llruEvict(theCache, index, tag, numBlocksPerSet, numBytesPerBlock, numAccesses, loadMisses, cycles);
				}
				else if (evictStrat == "fifo") {
					lfifoEvict(theCache, index, tag, numBlocksPerSet, numBytesPerBlock, numAccesses, loadMisses, cycles);
				} 
				else {
					cerr << "Invalid evict strategy" << endl;
				}
			}
			numLoads++;
			cycles++;
		}
		else if (command == "s") {
			bool found = sUpdateAtIndex(theCache, index, tag, numBlocksPerSet, numAccesses, storeHits, cycles, write);

			if (found == false) {
				found = doAllocate(theCache, index, tag, numBlocksPerSet, numBytesPerBlock, numAccesses, storeMisses, cycles, allocate);
			}

			if (found == false) {
				if (evictStrat == "lru") {
					slruEvict(theCache, index, tag, numBlocksPerSet, numBytesPerBlock, numAccesses, storeMisses, cycles, allocate);
				} 
				else if (evictStrat == "fifo") {
					sfifoEvict(theCache, index, tag, numBlocksPerSet, numBytesPerBlock, numAccesses, storeMisses, cycles, allocate);
				}
				else {
					cerr << "Invalid evict strategy" << endl;
				}
			}
			numStores++;
			cycles++;
		}
		numAccesses = numStores + numLoads;

	}

	cout << "Total loads: " << numLoads << endl;
	cout << "Total stores: " << numStores << endl;
	cout << "Load hits: " << loadHits << endl;
	cout << "Load misses: " << loadMisses << endl;
	cout << "Store hits: " << storeHits << endl;
	cout << "Store misses: " << storeMisses << endl;
	cout << "Total cycles: " << cycles << endl;
}
