/* Shawn Tripathy and Jose Delgado
 * CSF Assignment 3
 * helper.h
 * Header file for helper functions
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



struct block {
	bool valid;
	int tag;
	int index;
	bool dirty;
	int usedAt; //when block was used, updated every hit
	int broughtInAt; //load timestamp fifo
};

string convertHexToBinary(string address);

int checkInputs(int numSets, int numBlocks, int numBytes, string allocate, string write);

vector<vector<block>> createSetAssociative(int numSets, int numBlocks);

int getIndex(string address, int numTagBits, int numIndexBits);

int getTag(string address, int numTagBits);

bool lUpdateAtIndex(vector<vector<block>>& theCache, int index, int tag, int numBlocksPerSet, int numBytesPerBlock, int& loadHits, int& loadMisses, int& numAccesses, int& cycles);

void llruEvict(vector<vector<block>>& theCache, int index, int tag, int numBlocksPerSet, int numBytesPerBlock, int numAccesses, int& loadMisses, int& cycles);

void lfifoEvict(vector<vector<block>>& theCache, int index, int tag, int numBlocksPerSet, int numBytesPerBlock, int numAccesses, int& loadMisses, int& cycles);

bool sUpdateAtIndex(vector<vector<block>>& theCache, int index, int tag, int numBlocksPerSet, int numAccesses, int& storeHits, int& cycles, string write);

bool doAllocate(vector<vector<block>>& theCache, int index, int tag, int numBlocksPerSet, int numBytesPerBlock, int numAccesses, int& storeMisses, int& cycles, string allocate);

void slruEvict(vector<vector<block>>& theCache, int index, int tag, int numBlocksPerSet, int numBytesPerBlock, int numAccesses, int& storeMisses, int& cycles, string allocate);

void sfifoEvict(vector<vector<block>>& theCache, int index, int tag, int numBlocksPerSet, int numBytesPerBlock, int numAccesses, int& storeMisses, int& cycles, string allocate);

