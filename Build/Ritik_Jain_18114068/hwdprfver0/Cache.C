/** 
 * @author Ritik Jain
 * @date 26/10/2019
 * @file Cache.C
 * @brief Implements Data Cache Module
 */

#include "Cache.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

Cache::Cache(u_int32_t _numSets, u_int32_t _assoc, u_int32_t _blockSize, bool _randRep, bool _writeAlloc, bool _writeThrough)
{
    numSets = _numSets;
    assoc = _assoc;
    blockSize = _blockSize;
    randReplacement = _randRep;
    writeAlloc = _writeAlloc;
    writeThrough = _writeThrough;

    printf("Initializing Cache :\nSets : %u\nBlock Size :%u, Associativity :%u\n", numSets, blockSize, assoc);

    tags = (int**) calloc(numSets, sizeof(int*));
    valid = (bool**) calloc(numSets, sizeof(bool*));
    dirty = (bool**) calloc(numSets, sizeof(bool*));
    lru = (int**) calloc(numSets, sizeof(int*));

    for(int i=0; i < numSets; i++)
    {
        tags[i] = (int*)calloc(assoc,sizeof(int));
        valid[i] = (bool*)calloc(assoc,sizeof(bool));
        dirty[i] = (bool*)calloc(assoc,sizeof(bool));
        lru[i] = (int*)calloc(assoc,sizeof(int));
    }

    //Initialize Data
    reset();
}

Cache::~Cache()
{
    for(int i=0; i < numSets; i++)
    {
        free(tags[i]);
        free(valid[i]);
        free(dirty[i]);
        free(lru[i]);
    }

    free(tags);
    free(valid);
    free(dirty);
    free(lru);
}

void Cache::reset()
{
    for(int i=0; i < numSets; i++)
    {
        for(int j=0; j < assoc; j++)
        {
            tags[i][j] = lru[i][j] = 0;
            dirty[i][j] = valid[i][j] = false;
        }
    }

    //Reseed the random number generator
    srand(100);
}

bool Cache::check(u_int32_t addr, bool load)
{
    bool isHit = false;

    int b = (int) log2((double)blockSize);
    int s = (int) log2((double)numSets);
    int t = 32 - b - s;

    int index = ((addr >> b) <<  (32-s)) >> (32-s) ;
    int tag = addr >> (b+s);

    for( int i=0; i<assoc; i++)
    {
        if(tag == tags[index][i] && valid[index][i]) {
            isHit = true;
            break;
        }
    }

    return isHit;
}


u_int32_t Cache::getTag(u_int32_t addr) {
	int b = (int)log2((double)blockSize);
	int s = (int)log2((double)numSets);
	int t = 32 - b - s;

	u_int32_t tag = addr >> (b + s);
	return tag;
}

u_int32_t Cache::getIndex(u_int32_t addr) {
	int b = (int)log2((double)blockSize);
	int s = (int)log2((double)numSets);

	u_int32_t index = ((addr >> b) << (32 - s)) >> (32 - s);
	return index;
}

// accesses cache, returns true if we have a hit, false otherwise, don't change any state if modify == false
bool Cache::access(u_int32_t addr, bool load) {
	bool isHit = false;

	int b = (int)log2((double)blockSize);
	int s = (int)log2((double)numSets);
	int t = 32 - b - s;

	int index = ((addr >> b) << (32 - s)) >> (32 - s);
	int tag = addr >> (b + s);

	if(numSets == 1) index = 0;

	printf("Cache Accessed at : Index %d, Tag %d\n",index,tag);

	bool inCache = false;
	bool foundEmpty;

	// first check to see if the data is already in there
	for(int i = 0; i < assoc; ++i) {
		if(tag == tags[index][i] && valid[index][i]) { // found it
			isHit = true;

			if(!load)
				dirty[index][i] = true; // its dirty now that we stored to it

			// update LRU information (no need to do this for DM)
			if(assoc > 1 && !randReplacement) {
				// for all that had smaller LRU numbers, add 1 to them
				for(int j = 0; j < assoc; ++j) {
					if(lru[index][j] < lru[index][i] && valid[index][j]) 
						lru[index][j]++;
				}

				lru[index][i] = 0;
			}

			inCache = true;
			break; // no need to search more
		}
	}

	// if it wasn't in cache, bring it in if it was a load or if we are doing write allocate
	if(!inCache && (load || writeAlloc)) {
		foundEmpty = false;

		// first search to see if there is an open spot at that index
		for(int i = 0; i < assoc; ++i) {
			if(!valid[index][i]) { // found an open spot... all is good with the world
				tags[index][i] = tag;
				valid[index][i] = true;
				if(!load) dirty[index][i] = true;

				// need to update LRU bits if we are using that policy
				if(assoc > 1 && !randReplacement) {
					// for all that had smaller LRU numbers, add 1 to them
					for(int j = 0; j < assoc; ++j) {
						if(valid[index][j])  // since we found an open slot, we can just increase all the others without worrying
							lru[index][j]++;
					}

					lru[index][i] = 0;
				}

				foundEmpty = true;
				break;
			}
		}

		// if we couldn't find an empty spot, we need to kick out something old
		if(!foundEmpty) {
			// if DM, just put it in the only slot we have...
			if(assoc == 1) {
				tags[index][0] = tag;
				valid[index][0] = true;

				//if(dirty[index][0]) evicted = true;

				if(load) dirty[index][0] = false;
				else dirty[index][0] = true;
			}

			// just kick out a random guy if we specified random replacement policy
			else if(randReplacement) {
				int temp = rand() % assoc;
				tags[index][temp] = tag;
				valid[index][temp] = true;

				//if(dirty[index][temp]) evicted = true;

				if(load) dirty[index][temp] = false;
				else dirty[index][temp] = true;
			}

			// find LRU and kick him out
			else {
				for(int i = 0; i < assoc; ++i) {
					lru[index][i]++;
					if(lru[index][i] == assoc) { // this is the LRU
						tags[index][i] = tag;
						valid[index][i] = true;

						//if(dirty[index][i]) evicted = true;

						if(load) dirty[index][i] = false;
						else dirty[index][i] = true;

						lru[index][i] = 0;
					}
				}
			}
		}
	}

	return isHit;
}
