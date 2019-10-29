/**
 * @file memQueue.h
 * @date 24/10/2019
 * @author Ritik Jain
 */

#ifndef MEMQUEUE_H
#define MEMQUEUE_H

#include <sys/types.h>

class Cache;
struct Request;

//Implemented as a Circular Queue
class memQueue
{
    private :
        char id;
        u_int32_t capacity;
        u_int32_t latency;
        bool write; //True if write buffer, false if read queue
        bool pipelined; //True if pipelined

        Cache *source; //L2 cache where we receive requests from.

        u_int32_t front; //Front of the queue
        u_int32_t rear; //Rear of the queue
        u_int32_t size; //Size of the queue

        Request *queue;
        u_int32_t* readyTime; //The time when this entry will be ready
	    u_int32_t* tags; // the time when this entry will be ready
	    u_int32_t* indexes; // the time when this entry will be ready

        /* Finds if the request is a duplicate by : searching tags and indices */
        int findDuplicate(u_int32_t tag, u_int32_t index);

    public :


        /*
         * Initializes the memory request queue
         */
        memQueue(u_int32_t capacity, Cache* c, u_int32_t latency, bool pipelined, bool write, char name);

        /* Destructs the memory queue */
        ~memQueue();

        /* Prints the queue : For Debugging */
        void printQueue();

        /* Pushes a request into the queue */
        bool push(Request req, u_int32_t cycle);

        /* Pops a request out of the queue */
        bool pop();

        /* Peeks at the front element. Does not remove it */
        Request peek();

        /* Returns the size of the queue i.e. The number of requests in it */
        u_int32_t getSize();

        /* Check whether the front element is ready to go */
        bool frontReady(u_int32_t cycle);
};

#endif