/**
 * @author Ritik Jain
 * @date 25/10/2019
 * @file Cache.h
 * @brief Implements Write-Through/ Write-Back Pipelined/ Non-Pipelined Cache
 */

#ifndef CACHE_H
#define CACHE_H

#include <sys/types.h>

/*
 * Implements Cache with LRU Policy [Least Recently Used]
 */
class Cache
{
    private:
        
        //Data
        int** tags;
        bool** valid;
        bool** dirty;
        int** lru;

        u_int32_t blockSize; //Cache block size
        u_int32_t assoc; //Cache Associativity
        u_int32_t numSets; //Number of sets in the cache

        bool randReplacement; //Whether replacement follows LRU or is random
        bool writeAlloc; //Is the cache write allocated (write back)
        bool writeThrough; //Is the cache write through

    public :

        /* Initializes the Cache */
        Cache(u_int32_t numSets, u_int32_t assoc, u_int32_t blockSize, bool randRep, bool writeAlloc, bool writeThrough);

        /* Destructor for the Cache Module */
        ~Cache();

        /* Checks whether an element is in the cache or not (does not change the time-stamp */
        bool check( u_int32_t addr, bool load);

        /* Gets the tag of a memory address */
        u_int32_t getTag(u_int32_t addr);

        /* Gets the index of a memory address */
        u_int32_t getIndex(u_int32_t addr);

        //Accesses the cache, returns true if it is a hit, false otherwise. No change in state if modify==false */
        bool access( u_int32_t addr, bool load);

        //Resets the cache
        void reset();
};

#endif //CACHE_H